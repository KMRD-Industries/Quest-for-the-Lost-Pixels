#include "TextureSystem.h"
#include <fstream>
#include <iostream>
#include "Coordinator.h"
#include "Paths.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Image.hpp"
#include "TextureParser.h"
#include "TileComponent.h"
#include "Tileset.h"
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
    try
    {
        Tileset parsed_tileset = parseTileset(file_path); // Parse Tileset to struct
        sf::Image image;
        long gid = static_cast<long>(texture_map.size()); // Get id first unused tile id

        // Load image
        if (!image.loadFromFile(std::string(ASSET_PATH) + "/floorAtlas/" + parsed_tileset.image + ".png"))
        {
            throw std::runtime_error("Cannot open image: " + parsed_tileset.image);
        }

        // When loading maps, tiles are numerated from 0 and tilesets in system may have different starting IDs.
        // To synchronize map tilesets with loaded tilesets, it's essential to store the first ID of each loaded set
        // and then adjust map tilesets while loading.
        texture_indexes.emplace(parsed_tileset.name, gid);

        gid += 1; // First unused ID
        long first_gid_copy = gid;

        sf::Texture tex;
        tex.loadFromImage(image);
        textures.emplace(parsed_tileset.name, tex);

        // Read all tiles into system
        for (int y = 0; y < parsed_tileset.imageheight; y += parsed_tileset.tileheight)
        {
            for (int x = 0; x < parsed_tileset.imagewidth; x += parsed_tileset.tilewidth)
            {
                texture_map.emplace(gid++, sf::IntRect(x, y, parsed_tileset.tilewidth, parsed_tileset.tileheight));
            }
        }

        no_textures = static_cast<long>(texture_map.size());

        // Animations are also stored in TilesetFormat
        for (auto& tile : parsed_tileset.tiles)
        {
            long adjusted_id = first_gid_copy + tile.id;

            if (!tile.animation.empty())
            {
                std::vector<AnimationFrame> frames;

                for (auto& frame : tile.animation)
                {
                    long id = frame.tileid + 1;
                    long duration = frame.duration;
                    frames.push_back({id, duration});
                }

                map_animations.emplace(adjusted_id, frames);
            }

            if (!tile.objects.empty())
            {
                map_collisions.emplace(adjusted_id, tile.objects.at(0));
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


Collision TextureSystem::getCollision(const std::string& tileset_name, const long id)
{
    try
    {
        if (!texture_indexes.contains(tileset_name))
        {
            return Collision{};
        }

        long ad = id + texture_indexes.at(tileset_name);

        if (map_collisions.contains(ad))
        {
            return map_collisions.at(ad);
        }
        return Collision{};
    }
    catch (...)
    {
        return Collision{};
    }
}

std::vector<AnimationFrame> TextureSystem::getAnimations(const std::string& tileset_name, long id)
{
    try
    {
        return map_animations.at(id + texture_indexes.at(tileset_name));
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
