#include "MapSystem.h"
#include <nlohmann/json.hpp>
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Config.h"
#include "Coordinator.h"
#include "DoorComponent.h"
#include "EnemySystem.h"
#include "GameUtility.h"
#include "LootComponent.h"
#include "MapComponent.h"
#include "MapParser.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "PublicConfigMenager.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

/**
 * @brief Load room layout from given path of Tiled Json map format
 * @param path The path to JSON formatted Tiled map
 */
void MapSystem::loadMap(const std::string& path)
{
    // Remove old room entites before creating new ones
    resetMap();

    int index{};
    int x_position = {};
    int y_position = {};

    // Iterate over room layers
    for (const Map& parsedMap = parseMap(path); const Layer& layer : parsedMap.layers)
    {
        // Index to retrieve position of Tile. Layer contains all tiles in correct order,
        // and with number of tiles in rows and cols we can model room layout.
        index = {};

        // Process base64 layer data into vector of tiles
        for (const uint32_t tile : processDataString(layer.data, static_cast<size_t>(layer.width) * layer.height, 0))
        {
            m_flipFlags = (tile & m_mask) >> 28;
            m_tileID = tile & ~m_mask;

            x_position = index % layer.width;
            y_position = index / layer.width;

            if (m_tileID > 0)
            {
                processTile(m_tileID, m_flipFlags, layer.id, x_position, y_position, parsedMap);
            }

            index++;
        }
    }
}

/**
 * @brief tile according to given flag
 * @param flags - First 4 bits of tile description from Tiled Map
 * @param rotation - Tile rotation parameter from TransformComponent
 * @param scale - Tile scale parameter from Transformcomponent
 */
void MapSystem::doFlips(const std::uint8_t& flags, float& rotation, sf::Vector2f& scale)
{
    // 0000 = no change
    // 0100 = vertical = swap y-axis
    // 1000 = horizontal = swap x-axis
    // 1100 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    // 0010 = diag = rotate 90 degrees right and swap x-axis
    // 0110 = diag+vert = rotate 270 degrees right
    // 1010 = horiz+diag = rotate 90 degrees right
    // 1110 = horiz+vert+diag = rotate 90 degrees right and swap y-axis

    // Horizontal = 0x8;
    // Vertical = 0x4;
    // Diagonal = 0x2;

    switch (flags)
    {
    case GameType::VerticalFlip:
        scale.y *= -1;
        break;

    case GameType::HorizontalFlip:
        scale.x *= -1;
        break;

    case GameType::HorizontalVerticalFlip:
        scale = -scale;
        break;

    case GameType::DiagonalFlip:
        scale.x *= -1;
        rotation += config::ROTATION_90;
        break;

    case GameType::DiagonalVerticalFlip:
        rotation += config::ROTATION_270;
        break;

    case GameType::DiagonalHorizontalFlip:
        rotation += config::ROTATION_90;
        break;

    case GameType::AllFlips:
        rotation += config::ROTATION_90;
        scale.x *= -1;
        break;
    default:
        // No flip, do nothing
        break;
    }
}

/**
 * @brief Initialize Tile Components. Set up map position and all necessary stuff to display Tile correctly.
 * @param tileID The identifier of Tile Texture.
 * @param flipFlags The 4 bits describing flips of tile.
 * @param layerID The identifier of layer for correct order of room rendering.
 * @param xPos The position of tile in map.
 * @param yPos The position of tile in mao.
 * @param parsedMap The full map info.
 */
void MapSystem::processTile(const uint32_t tileID, const uint32_t flipFlags, const int layerID, const int xPos,
                            const int yPos, const Map& parsedMap)
{
    const Entity mapEntity = gCoordinator.createEntity();

    const auto tileComponent = TileComponent{
        static_cast<unsigned>(tileID - parsedMap.tilesets.at(findKeyLessThan(parsedMap.tilesets, tileID))),
        findKeyLessThan(parsedMap.tilesets, static_cast<signed>(tileID)), layerID};
    auto transformComponent = TransformComponent(getPosition(xPos + 1, yPos + 1, parsedMap.tileheight));

    doFlips(flipFlags, transformComponent.rotation, transformComponent.scale);

    gCoordinator.addComponents(mapEntity, RenderComponent{}, MapComponent{}, transformComponent, tileComponent);

    if (tileComponent.tileSet == "SpecialBlocks") // Handle special Tiles
    {
        switch (tileComponent.id)
        {
        case static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER):
            {
                gCoordinator.addComponent(mapEntity, DoorComponent{});
                auto& doorComponent = gCoordinator.getComponent<DoorComponent>(mapEntity);

                if (yPos == 0)
                    doorComponent.entrance = GameType::DoorEntraces::NORTH;
                else if (yPos == parsedMap.height - 1)
                    doorComponent.entrance = GameType::DoorEntraces::SOUTH;
                else if (xPos == 0)
                    doorComponent.entrance = GameType::DoorEntraces::WEST;
                else if (xPos == parsedMap.width - 1)
                    doorComponent.entrance = GameType::DoorEntraces::EAST;

                break;
            }

        case static_cast<int>(SpecialBlocks::Blocks::SPAWNERBLOCK):
            {
                if (!gCoordinator.hasComponent<SpawnerComponent>(mapEntity))
                    gCoordinator.addComponent(mapEntity, SpawnerComponent{.enemyType = Enemies::EnemyType::MELEE});

                break;
            }
        case static_cast<int>(SpecialBlocks::Blocks::BOSSSPAWNERBLOCK):
            {
                if (!gCoordinator.hasComponent<SpawnerComponent>(mapEntity))
                    gCoordinator.addComponent(mapEntity, SpawnerComponent{.enemyType = Enemies::EnemyType::BOSS});

                break;
            }
        case static_cast<int>(SpecialBlocks::Blocks::STARTINGPOINT):
            {
                const sf::Vector2f pos = getPosition(xPos, yPos, parsedMap.tileheight);
                GameUtility::startingPosition = {pos.x, pos.y};
                break;
            }
        case static_cast<int>(SpecialBlocks::Blocks::DOWNDOOR):
            {
                gCoordinator.addComponent(mapEntity, PassageComponent{.activePassage = true});
                break;
            }
        case static_cast<int>(SpecialBlocks::Blocks::CHESTSPAWNERBLOCK):
            {
                if (!gCoordinator.hasComponent<LootComponent>(mapEntity))
                    gCoordinator.addComponent(mapEntity, LootComponent{});

                break;
            }
        default:
            {
            }
        }
    }
}

std::string MapSystem::findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, const long i)
{
    std::string result;
    long act = 0;

    for (const auto& [first, second] : atlas_sets)
        if (second <= i && second >= act)
        {
            result = first;
            act = second;
        }
    return result;
}

void MapSystem::resetMap() const
{
    std::unordered_set<Entity> entityToKill{};

    for (const auto entity : m_entities)
    {
        if (!gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            if (auto* collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            {
                collisionComponent->toDestroy = true;
            }
            else
            {
                entityToKill.insert(entity);
            }
        }
    }

    gCoordinator.getRegisterSystem<CollisionSystem>()->deleteMarkedBodies();

    for (auto& entity : entityToKill) gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}

sf::Vector2f MapSystem::getPosition(const int x_axis, const int y_axis, const int map_tile_width) const
{
    return sf::Vector2f(static_cast<float>(x_axis), static_cast<float>(y_axis)) * static_cast<float>(map_tile_width) *
        configSingleton.GetConfig().gameScale;
}
