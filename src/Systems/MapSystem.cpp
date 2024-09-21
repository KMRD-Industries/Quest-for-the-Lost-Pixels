#include "MapSystem.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Config.h"
#include "Coordinator.h"
#include "DoorComponent.h"
#include "EnemySystem.h"
#include "GameUtility.h"
#include "MapComponent.h"
#include "MapParser.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MapSystem::init()
{
}

void MapSystem::update() {}

/**
 * @brief Load room layout from given path of Tiled Json map format
 * @param path The path to JSON formatted Tiled map
 */
void MapSystem::loadMap(const std::string& path)
{
    resetMap(); // Remove old room entities before creating new ones

    for (const Map& parsedMap = parseMap(path); const Layer& layer : parsedMap.layers)
    {
        // Index to retrieve position of Tile. Layer contains all tiles in correct order,
        // and with number of tiles in rows and cols we can model room layout.
        int index{};

        // Process base64 layer data into vector of tiles
        for (const uint32_t tile : processDataString(layer.data, static_cast<size_t>(layer.width) * layer.height, 0))
        {
            flipFlags = (tile & mask) >> 28;
            tileID = tile & ~mask;

            const int x_position = index % layer.width;
            const int y_position = index / layer.width;

            if (tileID > 0)
            {
                processTile(tileID, flipFlags, layer.id, x_position, y_position, parsedMap);
            }

            index++;
        }
    }
}

/**
 * @brief tile according to given flag
 * @param flags - First 4 bits of tile description from Tiled Map
 * @param rotation - Tile rotation parameter from TransformComponent
 * @param scale - Tile scale parameter from TransformComponent
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

    gCoordinator.addComponent(mapEntity, RenderComponent{});
    gCoordinator.addComponent(mapEntity, MapComponent{});
    gCoordinator.addComponent(mapEntity, transformComponent);
    gCoordinator.addComponent(mapEntity, tileComponent);

    if (tileComponent.tileSet == "SpecialBlocks") // Handle special Tiles
    {
        switch (tileComponent.id)
        {
        case static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER):
            {
                gCoordinator.addComponent(mapEntity, DoorComponent{});
                auto& doorComponent = gCoordinator.getComponent<DoorComponent>(mapEntity);

                if (yPos == 0)
                {
                    doorComponent.entrance = GameType::DoorEntraces::NORTH;
                }
                else if (yPos == parsedMap.height - 1)
                {
                    doorComponent.entrance = GameType::DoorEntraces::SOUTH;
                }
                else if (xPos == 0)
                {
                    doorComponent.entrance = GameType::DoorEntraces::WEST;
                }
                else if (xPos == parsedMap.width - 1)
                {
                    doorComponent.entrance = GameType::DoorEntraces::EAST;
                }

                break;
            }
        case static_cast<int>(SpecialBlocks::Blocks::SPAWNERBLOCK):
            {
                if (!gCoordinator.hasComponent<SpawnerComponent>(mapEntity))
                {
                    gCoordinator.addComponent(mapEntity, SpawnerComponent{});
                }

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
                gCoordinator.addComponent(mapEntity, PassageComponent{});
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
    {
        if (second <= i && second >= act)
        {
            result = first;
            act = second;
        }
    }
    return result;
}

void MapSystem::resetMap() const
{
    std::deque<Entity> entityToRemove;

    for (const auto& entity : m_entities)
    {
        if (!gCoordinator.hasComponent<PlayerComponent>(entity) && !gCoordinator.hasComponent<DoorComponent>(entity))
        {
            entityToRemove.push_back(entity);
        }
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.getRegisterSystem<CollisionSystem>()->deleteBody(entityToRemove.front());
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }

    entityToRemove.clear();
}

sf::Vector2f MapSystem::getPosition(const int x_axis, const int y_axis, const int map_tile_width)
{
    return sf::Vector2f(static_cast<float>(x_axis), static_cast<float>(y_axis)) * static_cast<float>(map_tile_width) *
        config::gameScale;
}
