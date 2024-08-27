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
#include "MapComponent.h"
#include "MapParser.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

/**
 * @brief Load room layout from given path of Tiled Json map format
 * @param path The path to JSON formatted Tiled map
 */
void MapSystem::loadMap(const std::string& path) const
{
    // Remove old room entites before creating new ones
    resetMap();

    // Iterate over room layers
    for (const Map& parsedMap = parseMap(path); const Layer& layer : parsedMap.layers)
    {
        // Index to retrieve position of Tile. Layer contains all tiles in correct order,
        // and with number of tiles in rows and cols we can model room layout.
        int index{};

        // Tiles info is stored in 32 bits format where first 4 bits describe flips of tile, and the rest stands for
        // tile id
        static constexpr std::uint32_t mask = 0xf0000000;

        // Process base64 layer data into vector of tiles
        for (const uint32_t tile : processDataString(layer.data, static_cast<size_t>(layer.width) * layer.height, 0))
        {
            // Decode flip flags and tile ID
            const uint32_t flipFlags = (tile & mask) >> 28;
            const uint32_t tileID = tile & ~mask;

            // Calculate Tile position
            const int x_position = index % layer.width;
            const int y_position = index / layer.width;

            if (tileID < 1)
            {
                index++;
                continue;
            }

            processTile(tileID, flipFlags, layer.id, x_position, y_position, parsedMap);
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
    case VerticalFlip:
        scale.y *= -1;
        break;

    case HorizontalFlip:
        scale.x *= -1;
        break;

    case HorizontalVerticalFlip:
        scale = -scale;
        break;

    case DiagonalFlip:
        scale.x *= -1;
        rotation += ROTATION_90;
        break;

    case DiagonalVerticalFlip:
        rotation += ROTATION_270;
        break;

    case DiagonalHorizontalFlip:
        rotation += ROTATION_90;
        break;

    case AllFlips:
        rotation += ROTATION_90;
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
    // Create new Entity which will describe Tile of map
    const Entity mapEntity = gCoordinator.createEntity();

    gCoordinator.addComponent(mapEntity, RenderComponent{});
    gCoordinator.addComponent(mapEntity, AnimationComponent{});
    gCoordinator.addComponent(mapEntity, ColliderComponent{});
    gCoordinator.addComponent(mapEntity, MapComponent{});

    TileComponent tile_component{};
    TransformComponent transform_component{};

    // Set up Tile Component
    tile_component.tileset = findKeyLessThan(parsedMap.tilesets, tileID);
    tile_component.id = tileID - parsedMap.tilesets.at(tile_component.tileset);
    tile_component.layer = layerID;

    // Set up the correct position, rotation, scale of tile
    transform_component.position = getPosition(xPos, yPos, parsedMap.tileheight);
    transform_component.scale = {1.f, 1.f};

    doFlips(flipFlags, transform_component.rotation, transform_component.scale);

    gCoordinator.addComponent(mapEntity, tile_component);
    gCoordinator.addComponent(mapEntity, transform_component);

    // Handle special Tiles
    if (tile_component.tileset == "SpecialBlocks")
    {
        switch (tile_component.id)
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
    const auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();

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
        CollisionSystem::deleteBody(entityToRemove.front());
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
