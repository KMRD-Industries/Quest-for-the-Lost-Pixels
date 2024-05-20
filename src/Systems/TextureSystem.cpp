#include "TextureSystem.h"
#include <fstream>
#include <iostream>
#include "Coordinator.h"
#include "Paths.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Image.hpp"
#include "TileComponent.h"
#include "Utils/Helpers.h"
#include "nlohmann/json.hpp"

extern Coordinator gCoordinator;

/**
 * This is to load TileSet from json file to atlas.
 * @param file_path path to TileSet
 * @return
 */
int TextureSystem::loadFromFile(const std::string& file_path)
{
    std::ifstream json_file(file_path);

    if (!json_file.is_open())
    {
        std::cout << "Failed to load TileSet." << std::endl;
        return 0;
    }

    nlohmann::json parsed_file = nlohmann::json::parse(json_file);
    std::string image_path = parsed_file["image"];

    sf::Image image;

    if (!image.loadFromFile(std::string(ASSET_PATH) + "/floorAtlass/" + extractFileName(image_path, "/", ".") + ".png"))
    {
        std::cout << "Failed to load image." << std::endl;
        return 0;
    }

    unsigned int width = parsed_file["imagewidth"];
    unsigned int height = parsed_file["imageheight"];
    int tile_width = parsed_file["tilewidth"];
    int tile_height = parsed_file["tileheight"];

    long gid = no_textures;
    std::string tileset_name = extractFileName(file_path, "/", ".");

    texture_indexes[tileset_name] = gid;

    gid = texture_map.size() + 1;
    long fist_gid_copy = gid;

    sf::Texture tex;
    tex.loadFromImage(image);

    textures[tileset_name] = tex;

    for (int y = 0; y < height; y += tile_height)
    {
        for (int x = 0; x < width; x += tile_width)
        {
            texture_map[gid++] = sf::IntRect(x, y, tile_width, tile_height);
        }
    }

    no_textures = texture_map.size();

    // Animation (MAP ONLY)
    auto it = parsed_file.find("tiles");
    if (it != parsed_file.end())
    {
        auto& tilesArray = parsed_file["tiles"];
        for (auto& tile : tilesArray)
        {
            auto& animationArray = tile["animation"];
            std::vector<sf::Sprite> frames;
            long firstId = -1;

            for (auto& frame : animationArray)
            {
                if (firstId < 0) firstId = frame["tileid"];
                long tileid = frame["tileid"];
                long adjusted_id = tileid + fist_gid_copy + 1;

                frames.push_back(getTile(tileset_name, tileid));
            }

            if (firstId > 0)
            {
                map_animations[firstId] = frames;
            }
        }
    }
    return 1;
}

void TextureSystem::loadTexturesFromFiles()
{
    const auto prefix = std::string(ASSET_PATH) + "/tileSets/";
    const auto sufix = ".json";
    for (const auto& texture : texture_files)
    {
        loadFromFile(prefix + texture + sufix);
    }
}

[[maybe_unused]] sf::Sprite TextureSystem::getTile(const std::string& tileset_name, long id)
{
    try
    {
        sf::Sprite s(textures[tileset_name], texture_map[id + texture_indexes[tileset_name]]);
        return s;
    }
    catch (...)
    {
        std::cout << "Texture ID out of range";
        return {};
    }
}


[[maybe_unused]] std::vector<sf::Sprite> TextureSystem::getAnimations(const std::string& tileset_name, long id)
{
    try
    {
        return map_animations[id + texture_indexes[tileset_name]];
    }
    catch (...)
    {
        std::cout << "Texture ID out of range";
        return {};
    }
}

void TextureSystem::loadTextures()
{
    for (const auto& entity : m_entities)
    {
        auto& tile_component = gCoordinator.getComponent<TileComponent>(entity);
        //        if (tile_component.id <= 0) continue;

        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& render_component = gCoordinator.getComponent<RenderComponent>(entity);

        render_component.sprite = getTile(tile_component.tileset, tile_component.id);
        render_component.layer = tile_component.layer;
    }
}
