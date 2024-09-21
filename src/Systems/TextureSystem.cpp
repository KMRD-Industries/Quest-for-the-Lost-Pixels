#include "TextureSystem.h"
#include <iostream>

#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "Paths.h"
#include "RenderComponent.h"
#include "TextureParser.h"
#include "TileComponent.h"
#include "Tileset.h"
#include "TransformComponent.h"
#include "SFML/Graphics/Image.hpp"
#include "Utils/Helpers.h"

extern Coordinator gCoordinator;

/**
 * Load JSON files to atlases.

 * @param path path to JSON Tile set.
 * @return status of operation
 */
int TextureSystem::loadFromFile(const std::string& path)
{
    try
    {
        const Tileset parsedTileSet = parseTileSet(path);

        // Get the first ID of newly loaded Set.
        long gid = initializeTileSet(parsedTileSet);
        const long firstGidCopy{gid};

        loadTilesIntoSystem(parsedTileSet, gid);
        loadAnimationsAndCollisionsIntoSystem(parsedTileSet, firstGidCopy);

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

/**
 * Initialize new Tile Set.
 * Load image from source, save Texture to map and return first tile ID.

 * @param parsedTileSet Tile Set parsed with nlohmann::json.
 * @return First Tile ID of a newly loaded set.
 */
long TextureSystem::initializeTileSet(const Tileset& parsedTileSet)
{
    sf::Image image;
    sf::Texture tex;

    // Get the first unused ID of the tile.
    long gid = static_cast<long>(m_mapTextureRects.size());

    // Load image from assets folder
    if (!image.loadFromFile(std::string(ASSET_PATH) + "/floorAtlas/" + parsedTileSet.image + ".png"))
        throw std::runtime_error("Cannot open image: " + parsedTileSet.image);

    // Store the starting ID (gid) of the loaded tile set.
    m_mapTextureIndexes.emplace(parsedTileSet.name, gid);

    // Load the image to Texture, and store it in a map.
    tex.loadFromImage(image);
    m_mapTextures.emplace(parsedTileSet.name, tex);

    return gid;
}

/**
 * Load all Tiles from parsed Set to a system.
 * Update the number of tiles in a system.

 * @param parsedTileSet Tile Set parsed with nlohmann::json.
 * @param gid Actually processed tile.
 */
void TextureSystem::loadTilesIntoSystem(const Tileset& parsedTileSet, long& gid)
{
    // Calculate the number of tiles to load.
    const int numTilesHorizontally = parsedTileSet.imagewidth / parsedTileSet.tilewidth;
    const int numTilesVertically = parsedTileSet.imageheight / parsedTileSet.tileheight;

    // Load all tiles into the system
    for (int row = 0; row < numTilesVertically; ++row)
        for (int col = 0; col < numTilesHorizontally; ++col)
            m_mapTextureRects.emplace(gid++,
                                      sf::IntRect(col * parsedTileSet.tilewidth, row * parsedTileSet.tileheight,
                                                  parsedTileSet.tilewidth, parsedTileSet.tileheight));

    m_lNoTextures = static_cast<long>(m_mapTextureRects.size());
}

/**
 * Load animations and collisions from a parsed tile set.

 * @param parsedTileSet Tile Set parsed with nlohmann::json.
 * @param firstGid First ID of a tile set.
 */
void TextureSystem::loadAnimationsAndCollisionsIntoSystem(const Tileset& parsedTileSet, const long& firstGid)
{
    for (const auto& [id, animation, objects] : parsedTileSet.tiles)
    {
        // Adjust tile with a tile set id.
        const long adjusted_id = firstGid + id;

        if (!animation.empty())
            m_mapAnimations.emplace(adjusted_id, animation);

        if (!objects.empty())
        {
            for (const auto& object : objects)
                // If an object contains more than one property, it's a weapon placement object.
                if (object.properties.size() >= 2)
                    m_mapWeaponPlacements.emplace(adjusted_id, object);
                else
                    m_mapCollisions.emplace(adjusted_id, object);
        }
    }
}

void TextureSystem::loadTexturesFromFiles()
{
    const auto prefix = std::string(ASSET_PATH) + "/tileSets/";
    const auto sufix = ".json";

    for (const auto& texture : m_vecTextureFiles)
        loadFromFile(prefix + texture + sufix);
}

sf::Sprite TextureSystem::getTile(const std::string& tileSetName, const long id) const
{
    try
    {
        sf::Sprite s(m_mapTextures.at(tileSetName), m_mapTextureRects.at(id + m_mapTextureIndexes.at(tileSetName)));
        return s;
    }
    catch (...)
    {
        std::cout << "ERROR::TEXTURE_SYSTEM::GET_TILE::COULD NOT GET SPRITE\n";
        return {};
    }
}

Collision TextureSystem::getCollision(const std::string& tileSetName, const long id)
{
    try
    {
        return m_mapCollisions.at(id + m_mapTextureIndexes.at(tileSetName));
    }
    catch (...)
    {
        // std::cout << "ERROR::TEXTURE_SYSTEM::GET_COLLISION::COULD NOT GET COLLISION\n";
        return {};
    }
}

std::vector<AnimationFrame> TextureSystem::getAnimations(const std::string& tileSetName, const long id)
{
    try
    {
        return m_mapAnimations.at(id + m_mapTextureIndexes.at(tileSetName));
    }
    catch (...)
    {
        std::cout << "ERROR::TEXTURE_SYSTEM::GET_ANIMATIONS::COULD NOT GET ANIMATIONS\n";
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
        if (tile_component.id <= 0 || tile_component.id > m_lNoTextures)
            continue;

        if (std::ranges::find(m_vecTextureFiles, tile_component.tileset) == m_vecTextureFiles.end())
            continue;

        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& render_component = gCoordinator.getComponent<RenderComponent>(entity);
        auto& collider_component = gCoordinator.getComponent<ColliderComponent>(entity);

        // Adjust tile index
        long adjusted_id = tile_component.id + m_mapTextureIndexes.at(tile_component.tileset);

        // Load animations from a system if tile is animated
        if (m_mapAnimations.contains(adjusted_id))
            animation_component.frames = getAnimations(tile_component.tileset, tile_component.id);

        if (m_mapCollisions.contains(adjusted_id))
        {
            auto& cc = m_mapCollisions.at(adjusted_id);
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            if (collider_component.body != nullptr && collider_component.collision != cc)
            {
                b2Vec2 offset = {static_cast<float>(cc.x - collider_component.collision.x),
                                 static_cast<float>(cc.y - collider_component.collision.y)};

                offset.x = convertPixelsToMeters(offset.x * config::gameScale);
                offset.y = convertPixelsToMeters(-offset.y * config::gameScale);

                collider_component.body->SetTransform(collider_component.body->GetPosition() + offset,
                                                      collider_component.body->GetAngle());

                transformComponent.position.x += convertMetersToPixel(offset.x);
                transformComponent.position.y += convertMetersToPixel(offset.y);
            }

            collider_component.collision = getCollision(tile_component.tileset, tile_component.id);
        }

        if (m_mapWeaponPlacements.contains(adjusted_id))
            collider_component.specialCollision = m_mapWeaponPlacements.at(adjusted_id);

        // Load texture of tile with that id to render component
        render_component.sprite = getTile(tile_component.tileset, tile_component.id);
        render_component.layer = tile_component.layer;
    }
}