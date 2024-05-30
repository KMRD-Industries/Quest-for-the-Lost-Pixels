#include "MapSystem.h"
#include <nlohmann/json.hpp>
#include "AnimationComponent.h"
#include "CollisionSystem.h"
#include "DoorComponent.h"
#include "Map.h"
#include "MapParser.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MapSystem::loadMap(const std::string& path)
{
    resetMap(); // Reset old map entities
    Map parsed_map = parseMap(path); // Parse json map

    auto start_iterator = m_entities.begin();
    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    auto textureSystem = gCoordinator.getRegisterSystem<TextureSystem>();

    for (auto& layer : parsed_map.layers)
    {
        if (layer.data.empty()) continue;

        static constexpr std::uint32_t mask = 0xf0000000;
        int index = {};

        for (uint32_t i : processDataString(layer.data, layer.width * layer.height, 0))
        {
            uint32_t flipFlags = (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            int layer_id = layer.id;
            int x_position = index % (static_cast<int>(layer.width));
            int y_position = index / (static_cast<int>(layer.width));

            if (tileID < 1)
            {
                index++;
                continue;
            }

            auto& tileComponent = gCoordinator.getComponent<TileComponent>(*start_iterator);
            auto& transform_component = gCoordinator.getComponent<TransformComponent>(*start_iterator);

            std::string tileset_name = findKeyLessThan(parsed_map.tilesets, tileID);
            tileID = tileID - parsed_map.tilesets.at(tileset_name) + 1;

            tileComponent.id = tileID;
            tileComponent.tileset = tileset_name;
            tileComponent.layer = layer_id;

            transform_component.position =
                sf::Vector2f(static_cast<float>(x_position), static_cast<float>(y_position)) *
                static_cast<float>(parsed_map.tileheight) * config::gameScale;
            doFlips(flipFlags, transform_component.rotation, transform_component.scale);

            Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileset_name, tileID);

            if (cc.width > 0 && cc.height > 0)
            {
                collisionSystem->createBody(*start_iterator, "Wall", {cc.width, cc.height},
                                            [](const GameType::CollisionData& entityT) {}, true, false, {cc.x, cc.y});
            }
            
            if (tileID == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER) + 1 && tileset_name == "SpecialBlocks")
            {
                collisionSystem->createBody(
                    *start_iterator, "Door", {parsed_map.tilewidth, parsed_map.tileheight},
                    [](const GameType::CollisionData& entityT) {}, true, false);
                gCoordinator.addComponent(*start_iterator, DoorComponent{});
                auto& doorComponent = gCoordinator.getComponent<DoorComponent>(*start_iterator);

                if (y_position == 0)
                    doorComponent.entrance = GameType::DoorEntraces::NORTH;
                else if (y_position == layer.height - 1)
                    doorComponent.entrance = GameType::DoorEntraces::SOUTH;
                if (x_position == 0)
                    doorComponent.entrance = GameType::DoorEntraces::WEST;
                else if (x_position == layer.width - 1)
                    doorComponent.entrance = GameType::DoorEntraces::EAST;
            }


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
    case 0x4: // 0100 = vertical flip
        scale.y *= -1;
        break;
    case 0x8: // 1000 = horizontal flip
        scale.x *= -1;
        break;
    case 0xC: // 1100 = horizontal and vertical flip
        scale = -scale;
        break;
    case 0x2: // 0010 = diagonal flip
        scale.x *= -1;
        rotation += 90.f;
        break;
    case 0x6: // 0110 = diagonal and vertical flip
        rotation += 270.f;
        break;
    case 0xA: // 1010 = diagonal and horizontal flip
        rotation += 90.f;
        break;
    case 0xE: // 1110 = diagonal, horizontal, and vertical flip
        rotation += 90.f;
        scale.x *= -1;
        break;
    default:
        // No flip, do nothing
        break;
    }
}

auto MapSystem::findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i) -> std::string
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

    for (const auto& entity : m_entities)
    {
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);

        collisionSystem->deleteBody(entity);

        transformComponent.position = {0.F, 0.F};
        tileComponent.id = {};
        tileComponent.layer = {};
        transformComponent.scale = sf::Vector2f(1.F, 1.F);
        transformComponent.rotation = {};
        animationComponent.frames.clear();
    }
}
