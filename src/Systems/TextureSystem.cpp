#include "TextureSystem.h"
#include <fstream>
#include <iostream>
#include "AnimationFrame.h"
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
 * @return succes or fail
 */
int TextureSystem::loadFromFile(const std::string& file_path)
{
    try
    {
        std::ifstream json_file(file_path);

        if (!json_file.is_open())
        {
            throw std::runtime_error("Cannot open file: " + file_path);
        }

        nlohmann::json parsed_file = nlohmann::json::parse(json_file);
        std::string image_path = parsed_file["image"];

        sf::Image image;

        if (!image.loadFromFile(std::string(ASSET_PATH) + "/floorAtlas/" + extractFileName(image_path, "/", ".") +
                                ".png"))
        {
            throw std::runtime_error("Cannot open image: " + image_path);
        }

        unsigned int width = parsed_file["imagewidth"];
        unsigned int height = parsed_file["imageheight"];
        int tile_width = parsed_file["tilewidth"];
        int tile_height = parsed_file["tileheight"];

        long gid = noTextures;
        std::string tileset_name = extractFileName(file_path, "/", ".");
        textureIndexes[tileset_name] = gid;

        gid = textureMap.size() + 1;
        long fist_gid_copy = gid;

        sf::Texture tex;
        tex.loadFromImage(image);

        textures[tileset_name] = tex;

        for (int y = 0; y < height; y += tile_height)
        {
            for (int x = 0; x < width; x += tile_width)
            {
                textureMap[gid++] = sf::IntRect(x, y, tile_width, tile_height);
            }
        }

        noTextures = textureMap.size();

        // Animation (MAP ONLY)
        if (auto it = parsed_file.find("tiles"); it != parsed_file.end())
        {
            for (auto& tile : parsed_file["tiles"])
            {
                long firstId = tile["id"];
                firstId += textureIndexes.at(tileset_name);
                std::vector<AnimationFrame> frames;

                for (auto& frame : tile["animation"])
                {
                    long id = frame["tileid"];
                    long duration = frame["duration"];
                    frames.push_back({id + 1, duration});
                }

                mapAnimations[firstId] = frames;
            }
        }
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught an exception: " << e.what() << std::endl;
        return 0;
    }
    catch (...)
    {
        std::cout << "Caught an unknown exception" << std::endl;
        return 1;
    }
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

sf::Sprite TextureSystem::getTile(const std::string& tileset_name, long id)
{
    try
    {
        sf::Sprite s(textures[tileset_name], textureMap[id + textureIndexes[tileset_name]]);
        return s;
    }
    catch (...)
    {
        std::cout << "Texture ID out of range";
        return {};
    }
}

std::vector<AnimationFrame> TextureSystem::getAnimations(const std::string& tileset_name, long id)
{
    try
    {
        return mapAnimations[id + textureIndexes[tileset_name]];
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

        if (tile_component.id <= 0 || tile_component.id > noTextures)
        {
            tile_component.layer = -1;
            tile_component.id = -1;
            continue;
        }

        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& render_component = gCoordinator.getComponent<RenderComponent>(entity);
        long adjusted_id = tile_component.id - 1 + textureIndexes.at(tile_component.tileset);

        if (mapAnimations.contains(adjusted_id + 1))
        {
            animation_component.frames = getAnimations(tile_component.tileset, tile_component.id);
            if (!animation_component.frames.empty())
            {
                animation_component.it = animation_component.frames.begin();
            }
            else
            {
                animation_component.it = animation_component.frames.end();
            }
        }

        render_component.sprite = getTile(tile_component.tileset, tile_component.id);
        render_component.layer = tile_component.layer;
    }
}
