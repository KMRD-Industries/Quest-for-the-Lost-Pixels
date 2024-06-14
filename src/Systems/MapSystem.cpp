#include "MapSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "DoorComponent.h"
#include "MapParser.h"
#include "PlayerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MapSystem::loadMap(std::string& path)
{
    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    resetMap();

    Map parsed_map = parseMap(path);
    auto start_iterator = m_entities.begin();

    for (auto& layer : parsed_map.layers)
    {
        if (layer.data.empty())
        {
            continue;
        }

        static constexpr std::uint32_t mask = 0xf0000000;
        int index = {};

        for (uint32_t i : processDataString(layer.data, layer.width * layer.height, 0))
        {
            uint32_t flipFlags = (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            int x_position = index % (static_cast<int>(layer.width));
            int y_position = index / (static_cast<int>(layer.width));

            if (tileID < 1)
            {
                index++;
                continue;
            }

            auto& tile_component = gCoordinator.getComponent<TileComponent>(*start_iterator);
            auto& transform_component = gCoordinator.getComponent<TransformComponent>(*start_iterator);

            processTile(start_iterator, tileID, flipFlags, layer.id, x_position, y_position, parsed_map);

            ++start_iterator;
            index++;
        }
    }
}

void MapSystem::doFlips(std::uint8_t flags, float& rotation, sf::Vector2f& scale)
{
    // 0000 = no change
    // 0100 = vertical = swap y axis
    // 1000 = horizontal = swap x axis
    // 1100 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    // 0010 = diag = rotate 90 degrees right and swap x axis
    // 0110 = diag+vert = rotate 270 degrees right
    // 1010 = horiz+diag = rotate 90 degrees right
    // 1110 = horiz+vert+diag = rotate 90 degrees right and swap y axis

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

void MapSystem::processTile(auto& entityIterator, uint32_t tileID, const uint32_t flipFlags, const int layerID,
                            const int xPos, const int yPos, const Map& parsed_map)
{
    auto& tile_component = gCoordinator.getComponent<TileComponent>(*entityIterator);
    auto& transform_component = gCoordinator.getComponent<TransformComponent>(*entityIterator);

    std::string tileset_name = findKeyLessThan(parsed_map.tilesets, tileID);
    tileID = tileID - parsed_map.tilesets.at(tileset_name) + 1;

    tile_component.id = tileID;
    tile_component.tileset = tileset_name;
    tile_component.layer = layerID;

    transform_component.position = getPosition(xPos, yPos, parsed_map.tileheight);
    doFlips(flipFlags, transform_component.rotation, transform_component.scale);

    if (tileset_name == "SpecialBlocks")
    {
        if (tileID == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER) + 1)
        {
            gCoordinator.addComponent(*entityIterator, DoorComponent{});
            auto& doorComponent = gCoordinator.getComponent<DoorComponent>(*entityIterator);

            if (yPos == 0)
                doorComponent.entrance = GameType::DoorEntraces::NORTH;
            else if (yPos == parsed_map.height - 1)
                doorComponent.entrance = GameType::DoorEntraces::SOUTH;
            if (xPos == 0)
                doorComponent.entrance = GameType::DoorEntraces::WEST;
            else if (xPos == parsed_map.width - 1)
                doorComponent.entrance = GameType::DoorEntraces::EAST;
        }
    };
}

std::string MapSystem::findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i)
{
    std::string result;
    long act = 0;

    for (const auto& pair : atlas_sets)
    {
        if (pair.second <= i && pair.second >= act)
        {
            result = pair.first;
            act = pair.second;
        }
    }
    return result;
}

void MapSystem::resetMap()
{
    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();

    for (auto& entity : m_entities)
    {
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        if (gCoordinator.hasComponent<PlayerComponent>(entity) || gCoordinator.hasComponent<DoorComponent>(entity))
        {
            continue;
        }

        collisionSystem->deleteBody(entity);

        transformComponent.position = {0.f, 0.f};
        tileComponent.id = {};
        tileComponent.layer = {};

        transformComponent.scale = sf::Vector2f(1.F, 1.F);
        transformComponent.rotation = {};
    }
}

sf::Vector2f MapSystem::getPosition(int x_axis, int y_axis, int map_tile_width)
{
    return sf::Vector2f(static_cast<float>(x_axis), static_cast<float>(y_axis)) * static_cast<float>(map_tile_width) *
        config::gameScale;
}