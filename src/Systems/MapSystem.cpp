#include "MapSystem.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Config.h"
#include "Coordinator.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MapSystem::loadMap(std::string& path)
{
    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();

    // Reset Map Entities to default
    for (const auto& entity : m_entities)
    {
        auto& map_component = gCoordinator.getComponent<TileComponent>(entity);
        auto& transform_component = gCoordinator.getComponent<TransformComponent>(entity);

        collisionSystem->deleteBody(entity);
        map_component.id = {};
        map_component.layer = {};
        transform_component.scale = sf::Vector2f(1.f, 1.f);
        transform_component.rotation = {};
    }

    std::ifstream json_file(path);

    if (!json_file.is_open())
    {
        return;
    }

    std::unordered_map<std::string, long> atlas_sets;
    nlohmann::json parsed_file = nlohmann::json::parse(json_file);

    for (auto tileset : parsed_file["tilesets"])
    {
        std::string name = extractFileName(tileset["source"], "/", ".");
        long first_gid = tileset["firstgid"];

        atlas_sets[name] = first_gid;
    }

    auto start_iterator = m_entities.begin();
    ++start_iterator;

    long width = {};
    long height = {};
    float tile_height = parsed_file["tileheight"];
    float tile_width = parsed_file["tilewidth"];

    for (auto& data : parsed_file["layers"])
    {
        if (!data.contains("data")) continue;

        static constexpr std::uint32_t mask = 0xf0000000;
        int index = {};

        width = data["width"];
        height = data["height"];

        for (uint32_t i : processDataString(data["data"], width * height, 0))
        {
            uint32_t flipFlags = (i & mask) >> 28;
            uint32_t tileID = i & ~mask;

            int layer = data["id"];
            int x_position = index % (static_cast<int>(width));
            int y_position = index / (static_cast<int>(width));

            if (tileID < 1)
            {
                index++;
                continue;
            }

            auto& tileComponent = gCoordinator.getComponent<TileComponent>(*start_iterator);
            auto& transform_component = gCoordinator.getComponent<TransformComponent>(*start_iterator);

            std::string tileset_name = findKeyLessThan(atlas_sets, tileID);
            tileID = tileID - atlas_sets[tileset_name] + 1;

            tileComponent.id = tileID;
            tileComponent.tileset = tileset_name;
            tileComponent.layer = layer;

            transform_component.position =
                sf::Vector2f(static_cast<float>(x_position), static_cast<float>(y_position)) * tile_height *
                config::gameScale;
            doFlips(flipFlags, transform_component.rotation, transform_component.scale);

            if (tileset_name == "SpecialBlocks")
            {
                if (tileID == static_cast<int>(SpecialBlocks::Blocks::STATICWALLCOLLIDER) + 1)
                    collisionSystem->createBody(
                        *start_iterator, "Wall", {tile_width, tile_height},
                        [](const GameType::CollisionData& entityT) {}, true, false);
                else if (tileID == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER) + 1)
                    collisionSystem->createBody(
                        *start_iterator, "Door", {tile_width, tile_height},
                        [](const GameType::CollisionData& entityT) {}, true, false);
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