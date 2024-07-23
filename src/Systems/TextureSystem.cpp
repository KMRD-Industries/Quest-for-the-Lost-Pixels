#include "TextureSystem.h"
#include <iostream>
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "DoorComponent.h"
#include "Paths.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Image.hpp"
#include "TextureParser.h"
#include "TileComponent.h"
#include "Tileset.h"
#include "Utils/Helpers.h"

extern Coordinator gCoordinator;

/**
 * This is to load TileSet from json file to atlas.

 * @param file_path path to TileSet
 * @return success or fail
 */
int TextureSystem::loadFromFile(const std::string& path)
{
    try
    {
        Tileset parsed_tileset = parseTileset(path); // Parse Tileset to struct
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
        std::cout << "Caught an exception: " << e.what() << '\n';
        return 0;
    }
    catch (...)
    {
        std::cout << "Caught an unknown exception" << '\n';
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
        sf::Sprite s(textures.at(tileset_name), texture_map.at(id + texture_indexes.at(tileset_name)));
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
    if (texture_indexes.find(tileset_name) == texture_indexes.end())
    {
        return Collision{};
    }

    long ad = id + texture_indexes.at(tileset_name);

    if (map_collisions.find(ad) != map_collisions.end())
    {
        return map_collisions.at(ad);
    }

    return Collision{};
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
/**
 * Load textures into tiles.
 */
void TextureSystem::loadTextures()
{
    for (const auto entity : m_entities)
    {
        auto& tile_component = gCoordinator.getComponent<TileComponent>(entity);

        // Ignore invalid values
        if (tile_component.id <= 0 || tile_component.id > no_textures)
        {
            continue;
        }

        if (std::find(texture_files.begin(), texture_files.end(), tile_component.tileset) == texture_files.end())
        {
            continue;
        }

        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& render_component = gCoordinator.getComponent<RenderComponent>(entity);
        auto& collider_component = gCoordinator.getComponent<ColliderComponent>(entity);

        // Adjust tile index
        long adjusted_id = tile_component.id + texture_indexes.at(tile_component.tileset);

        // Load animations from system if tile is animated
        if (map_animations.contains(adjusted_id))
        {
            animation_component.frames = getAnimations(tile_component.tileset, tile_component.id);
            animation_component.it = animation_component.frames.begin() + 1;
        }

        if (map_collisions.contains(adjusted_id))
        {
            collider_component.collision = getCollision(tile_component.tileset, tile_component.id);
        }

        // Load texture of tile with that id to render component
        render_component.sprite = getTile(tile_component.tileset, tile_component.id);
        render_component.layer = tile_component.layer;
    }
}
