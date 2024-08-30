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

 * @param path path to TileSet
 * @return success or fail
 */
int TextureSystem::loadFromFile(const std::string& path)
{
    try
    {
        Tileset parsed_tileset = parseTileset(path); // Parse Tileset to struct
        sf::Image image;

        long gid = static_cast<long>(m_mapTextureRects.size()); // Get id first unused tile id

        // Load image
        if (!image.loadFromFile(std::string(ASSET_PATH) + "/floorAtlas/" + parsed_tileset.image + ".png"))
        {
            throw std::runtime_error("Cannot open image: " + parsed_tileset.image);
        }

        // When loading maps, tiles are numerated from 0 and tilesets in system may have different starting IDs.
        // To synchronize map tilesets with loaded tilesets, it's essential to store the first ID of each loaded set
        // and then adjust map tilesets while loading.
        m_mapTextureIndexes.emplace(parsed_tileset.name, gid);

        const long first_gid_copy = gid;

        sf::Texture tex;
        tex.loadFromImage(image);

        m_mapTextures.emplace(parsed_tileset.name, tex);
        m_mapTexturesWithColorSchemeApplied.emplace(parsed_tileset.name, tex);

        // Calculate no Tiles to read from file
        const int numTilesHorizontaly = parsed_tileset.imagewidth / parsed_tileset.tilewidth;
        const int numTilesVertically = parsed_tileset.imageheight / parsed_tileset.tileheight;

        // Read all tiles into system
        for (int y = 0; y < numTilesVertically; y += 1)
        {
            for (int x = 0; x < numTilesHorizontaly; x += 1)
            {
                m_mapTextureRects.emplace(gid++,
                                          sf::IntRect(x * parsed_tileset.tilewidth, y * parsed_tileset.tileheight,
                                                      parsed_tileset.tilewidth, parsed_tileset.tileheight));
            }
        }

        m_lNoTextures = static_cast<long>(m_mapTextureRects.size());

        // Animations are also stored in TilesetFormat
        for (auto& [id, animation, objects] : parsed_tileset.tiles)
        {
            long adjusted_id = first_gid_copy + id;

            if (!animation.empty())
            {
                std::vector<AnimationFrame> frames;

                for (const auto& [tileid, duration, rotation] : animation)
                {
                    frames.push_back({tileid, duration});
                }

                m_mapAnimations.emplace(adjusted_id, frames);
            }

            if (!objects.empty())
            {
                m_mapCollisions.emplace(adjusted_id, objects.at(0));
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
    for (const auto& texture : m_vecTextureFiles)
    {
        loadFromFile(prefix + texture + sufix);
    }
}

sf::Sprite TextureSystem::getTile(const std::string& tileset_name, long id)
{
    try
    {
        sf::Sprite s(m_mapTexturesWithColorSchemeApplied.at(tileset_name),
                     m_mapTextureRects.at(id + m_mapTextureIndexes.at(tileset_name)));
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
    if (m_mapTextureIndexes.find(tileset_name) == m_mapTextureIndexes.end())
    {
        return Collision{};
    }

    const long ad = id + m_mapTextureIndexes.at(tileset_name);

    if (m_mapCollisions.find(ad) != m_mapCollisions.end())
    {
        return m_mapCollisions.at(ad);
    }

    return Collision{};
}

std::vector<AnimationFrame> TextureSystem::getAnimations(const std::string& tileset_name, long id)
{
    try
    {
        return m_mapAnimations.at(id + m_mapTextureIndexes.at(tileset_name));
    }
    catch (...)
    {
        // std::cout << "Texture ID out of range";
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
        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& render_component = gCoordinator.getComponent<RenderComponent>(entity);
        auto& collider_component = gCoordinator.getComponent<ColliderComponent>(entity);

        // Ignore invalid values
        if (tile_component.id < 0 || tile_component.id > m_lNoTextures)
        {
            continue;
        }

        if (std::find(m_vecTextureFiles.begin(), m_vecTextureFiles.end(), tile_component.tileset) ==
            m_vecTextureFiles.end())
        {
            continue;
        }

        // Adjust tile index
        long adjusted_id = tile_component.id + m_mapTextureIndexes.at(tile_component.tileset);

        // Load animations from system if tile is animated
        if (m_mapAnimations.contains(adjusted_id))
        {
            animation_component.frames = getAnimations(tile_component.tileset, tile_component.id);
            animation_component.it = {animation_component.frames.begin(), animation_component.frames.end()};
        }

        if (m_mapCollisions.contains(adjusted_id))
        {
            collider_component.collision = getCollision(tile_component.tileset, tile_component.id);
        }

        // Load texture of tile with that id to render component
        render_component.sprite = getTile(tile_component.tileset, tile_component.id);
        render_component.layer = tile_component.layer;
    }
}

void TextureSystem::modifyColorScheme(const int playerFloor)
{
    // One floor layout can be used for multiple levels
    const auto floorId = m_mapDungeonLevelToFloorInfo.at(playerFloor);
    const std::string& tileSet = m_mapFloorToTextureFile.at(floorId);

    // Copy image to apply color scheme
    sf::Image image = m_mapTextures.at(tileSet).copyToImage();

    const sf::Vector2u size = image.getSize();
    const ColorBalance& balance = m_mapColorScheme.at(playerFloor);

    auto applySemitoneFilter = [](const int value, const int shift) -> int
    {
        return std::min(255, std::max(0, value + shift * 2)); // Simplified approximation for semitone effect
    };

    // Apply semitone filter for each pixel
    for (unsigned int x = 0; x < size.x; ++x)
    {
        for (unsigned int y = 0; y < size.y; ++y)
        {
            sf::Color pixel = image.getPixel(x, y);

            const int r = applySemitoneFilter(pixel.r, balance.redBalance);
            const int g = applySemitoneFilter(pixel.g, balance.greenBalance);
            const int b = applySemitoneFilter(pixel.b, balance.blueBalance);

            pixel.r = static_cast<sf::Uint8>(r);
            pixel.g = static_cast<sf::Uint8>(g);
            pixel.b = static_cast<sf::Uint8>(b);

            image.setPixel(x, y, pixel);
        }
    }

    // Replace texture with applied filter
    m_mapTexturesWithColorSchemeApplied.at(tileSet).loadFromImage(image);

    // Change bg color
    std::string hexColor = colorToString(floorId);
    hexColor = hexColor.substr(1);

    int hexValue;
    std::stringstream ss;
    ss << std::hex << hexColor;
    ss >> hexValue;

    // Read rbg values and apply same filter
    int red = hexValue >> 16 & 0xFF;
    int green = hexValue >> 8 & 0xFF;
    int blue = hexValue & 0xFF;

    red = applySemitoneFilter(red, balance.redBalance);
    green = applySemitoneFilter(green, balance.greenBalance);
    blue = applySemitoneFilter(blue, balance.blueBalance);

    std::stringstream stringHex;
    stringHex << "#" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << red << std::setw(2)
              << std::setfill('0') << std::hex << std::uppercase << green << std::setw(2) << std::setfill('0')
              << std::hex << std::uppercase << blue;

    m_currentBackgroundColor = stringHex.str();
}

std::string TextureSystem::getBackgroundColor() { return m_currentBackgroundColor; }
