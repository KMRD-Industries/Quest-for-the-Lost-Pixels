#include "TextureSystem.h"

#include <DirtyFlagComponent.h>
#include <iomanip>
#include <iostream>

#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "GameTypes.h"
#include "Paths.h"
#include "PublicConfigMenager.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Image.hpp"
#include "TextureParser.h"
#include "TileComponent.h"
#include "Tileset.h"
#include "TransformComponent.h"
#include "Utils/Helpers.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

TextureSystem::TextureSystem() { loadTexturesFromFiles(); }

void TextureSystem::update() {}

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
    m_mapTextures.emplace(parsedTileSet.name, sf::Texture(tex));
    m_mapTexturesWithColorSchemeApplied.emplace(parsedTileSet.name, sf::Texture(tex));

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
    {
        for (int col = 0; col < numTilesHorizontally; ++col)
        {
            sf::VertexArray vertexArray(sf::PrimitiveType::Quads);

            // Calculate texture coordinates for VertexArray
            const auto texLeft = static_cast<float>(col * parsedTileSet.tilewidth);
            const auto texTop = static_cast<float>((row + 1) * parsedTileSet.tileheight);
            const auto texRight = static_cast<float>((col + 1) * parsedTileSet.tilewidth);
            const auto texBottom = static_cast<float>(row * parsedTileSet.tileheight);

            vertexArray.append(sf::Vertex({}, sf::Color::White, sf::Vector2f(texLeft, texTop))); // Top-left
            vertexArray.append(sf::Vertex({}, sf::Color::White, sf::Vector2f(texRight, texTop))); // Top-right
            vertexArray.append(sf::Vertex({}, sf::Color::White, sf::Vector2f(texRight, texBottom))); // Bottom-right
            vertexArray.append(sf::Vertex({}, sf::Color::White, sf::Vector2f(texLeft, texBottom))); // Bottom-left

            m_mapTextureRects.emplace(gid, vertexArray);

            // Do the same for sf::Sprite
            sf::IntRect spriteRectangle = {static_cast<int>(texLeft), static_cast<int>(texBottom),
                                           parsedTileSet.tilewidth, parsedTileSet.tileheight};

            m_mapTextureIntRects.emplace(gid++, spriteRectangle);
        }
    }

    m_lNoTextures = static_cast<long>(m_mapTextureRects.size());
}

/**
 * Load animations and collisions from a parsed tile set.

 * @param parsedTileSet Tile Set parsed with nlohmann::json.
 * @param firstGid First ID of a tile set.
 */
void TextureSystem::loadAnimationsAndCollisionsIntoSystem(const Tileset& parsedTileSet, const long& firstGid)
{
    for (const auto& [id, properties, animation, objects] : parsedTileSet.tiles)
    {
        // Adjust tile with a tile set id.
        const long adjusted_id = firstGid + id;

        if (!animation.empty()) m_mapAnimations.emplace(adjusted_id, animation);

        if (!objects.empty())
        {
            for (const auto& object : objects)
                // If an object contains more than one property, it's a weapon placement object.
                if (object.properties.size() >= 2)
                {
                    for (auto& property : object.properties)
                    {
                        if (property.name == "helmetPlacement")
                        {
                            m_mapHelmetPlacements.emplace(adjusted_id, object);
                            if (id <= 180) m_helmets.push_back(id);
                        }

                        if (property.name == "weaponPlacement")
                        {
                            m_mapWeaponPlacements.emplace(adjusted_id, object);
                            if (id <= 180)
                                m_weaponsIDs.emplace_back(id, GameType::stringToWeaponType(properties[0].value));
                        }

                        if (property.name == "bodyArmourPlacement")
                        {
                            m_mapBodyArmourPlacement.emplace(adjusted_id, object);
                            if (id <= 180) m_bodyArmours.push_back(id);
                        }
                    }
                }
                else
                {
                    m_mapCollisions.emplace(adjusted_id, object);
                }
        }
    }
}

void TextureSystem::loadTexturesFromFiles()
{
    const auto prefix = std::string(ASSET_PATH) + "/tileSets/";
    const auto sufix = ".json";

    for (const auto& texture : m_setTextureFiles) loadFromFile(prefix + texture + sufix);
}

sf::Sprite TextureSystem::getSprite(const std::string& tileSetName, const long id) const
{
    const auto textureIter = m_mapTexturesWithColorSchemeApplied.find(tileSetName);
    const auto indexIter = m_mapTextureIndexes.find(tileSetName);

    if (textureIter != m_mapTexturesWithColorSchemeApplied.end() && indexIter != m_mapTextureIndexes.end())
    {
        const auto rectIter = m_mapTextureIntRects.find(id + indexIter->second);
        if (rectIter != m_mapTextureIntRects.end()) return {textureIter->second, rectIter->second};

        // std::cout << "ERROR::TEXTURE_SYSTEM::GET_TILE::COULD NOT GET SPRITE\n";
        return {};
    }
}

sf::VertexArray TextureSystem::getTile(const std::string& tileSetName, const long id) const
{
    const auto textureIter = m_mapTexturesWithColorSchemeApplied.find(tileSetName);
    const auto indexIter = m_mapTextureIndexes.find(tileSetName);

    if (textureIter != m_mapTexturesWithColorSchemeApplied.end() && indexIter != m_mapTextureIndexes.end())
    {
        if (const auto rectIter = m_mapTextureRects.find(id + indexIter->second); rectIter != m_mapTextureRects.end())
        {
            return rectIter->second;
        }
    }

    // std::cout << "ERROR::TEXTURE_SYSTEM::GET_TILE::COULD NOT GET VERTEX\n";
    return {};
}

Collision TextureSystem::getCollision(const std::string& tileSetName, const long id)
{
    auto indexIter = m_mapTextureIndexes.find(tileSetName);

    if (indexIter != m_mapTextureIndexes.end())
    {
        auto collisionIter = m_mapCollisions.find(id + indexIter->second);
        if (collisionIter != m_mapCollisions.end()) return collisionIter->second;
    }

    // std::cout << "ERROR::TEXTURE_SYSTEM::GET_COLLISION::COULD NOT GET COLLISION\n";
    return {};
}

std::vector<AnimationFrame> TextureSystem::getAnimations(const std::string& tileSetName, const long id)
{
    auto indexIter = m_mapTextureIndexes.find(tileSetName);

    if (indexIter != m_mapTextureIndexes.end())
    {
        auto animIter = m_mapAnimations.find(id + indexIter->second);
        if (animIter != m_mapAnimations.end()) return animIter->second;
    }

    // std::cout << "ERROR::TEXTURE_SYSTEM::GET_ANIMATIONS::COULD NOT GET ANIMATIONS\n";
    return {};
}

/**
 * Load textures into tiles.
 */
void TextureSystem::loadTextures()
{
    for (const auto entity : m_entities)
    {
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        if (!gCoordinator.hasComponent<DirtyFlagComponent>(entity)) continue;

        if (auto it = m_mapTexturesWithColorSchemeApplied.find(tileComponent.tileSet);
            it != m_mapTexturesWithColorSchemeApplied.end())
            renderComponent.texture = &it->second;
        else
            continue;

        long adjusted_id = tileComponent.id + m_mapTextureIndexes.at(tileComponent.tileSet);

        // Load Animation with new ID
        if (auto animationsIter = m_mapAnimations.find(adjusted_id); animationsIter != m_mapAnimations.end())
        {
            if (!gCoordinator.hasComponent<AnimationComponent>(entity))
                gCoordinator.addComponent(entity, AnimationComponent{});

            auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
            auto& stateAnimationLookup = animation_component.stateToAnimationLookup;

            if (stateAnimationLookup.empty())
            {
                animation_component.frames[AnimationStateMachine::AnimationState::Idle] =
                    getAnimations(tileComponent.tileSet, tileComponent.id);
            }
            else
            {
                for (const auto& [fst, snd] : stateAnimationLookup)
                    animation_component.frames[fst] = getAnimations(snd.first, snd.second);
            }
        }

        if (auto animationComp = gCoordinator.tryGetComponent<AnimationComponent>(entity))
        {
            auto& stateAnimationLookup = animationComp->stateToAnimationLookup;
            if (!stateAnimationLookup.empty())
            {
                for (const auto& [fst, snd] : stateAnimationLookup)
                    animationComp->frames[fst] = getAnimations(snd.first, snd.second);
            }
        }

        // Load Collisions with new ID && adjust id by new margin
        if (auto collisionsIter = m_mapCollisions.find(adjusted_id); collisionsIter != m_mapCollisions.end())
        {
            if (!gCoordinator.hasComponent<ColliderComponent>(entity))
                gCoordinator.addComponent(entity, ColliderComponent{});

            auto& cc = m_mapCollisions.at(adjusted_id);
            // auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

            if (colliderComponent.body != nullptr && colliderComponent.collision != cc)
            {
                if (cc.x != colliderComponent.collision.x || cc.y != colliderComponent.collision.y)
                {
                    b2Vec2 offset = {(cc.x - colliderComponent.collision.x), (cc.y - colliderComponent.collision.y)};

                    offset.x = convertPixelsToMeters(offset.x * config::gameScale);
                    offset.y = convertPixelsToMeters(-offset.y * config::gameScale);

                    // colliderComponent.body->SetTransform(colliderComponent.body->GetPosition() + offset, 0);

                    // transformComponent.position.x += convertMetersToPixel(offset.x);
                    // transformComponent.position.y += convertMetersToPixel(offset.y);
                }
            }

            colliderComponent.collision = cc;
        }

        // Load Helmet Placement Collision
        if (auto helmetPlacementsIter = m_mapHelmetPlacements.find(adjusted_id);
            helmetPlacementsIter != m_mapHelmetPlacements.end())
        {
            auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
            colliderComponent.helmetPlacement = m_mapHelmetPlacements.at(adjusted_id);
        }

        // Load Weapon Placement Collision
        if (auto weaponPlacementsIter = m_mapWeaponPlacements.find(adjusted_id);
            weaponPlacementsIter != m_mapWeaponPlacements.end())
        {
            auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
            colliderComponent.weaponPlacement = m_mapWeaponPlacements.at(adjusted_id);
        }

        // Load Body Armour Placement
        if (auto bodyArmourPlacementsIter = m_mapBodyArmourPlacement.find(adjusted_id);
            bodyArmourPlacementsIter != m_mapBodyArmourPlacement.end())
        {
            auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
            colliderComponent.bodyArmourPlacement = m_mapBodyArmourPlacement.at(adjusted_id);
        }

        renderComponent.sprite = getSprite(tileComponent.tileSet, tileComponent.id);
        renderComponent.vertexArray = getTile(tileComponent.tileSet, tileComponent.id);
    }
}


void TextureSystem::modifyColorScheme(const int playerFloor)
{
    // One floor layout can be used for multiple levels
    const auto floorId = config::m_mapDungeonLevelToFloorInfo.at(playerFloor);
    const std::string& tileSet = config::m_mapFloorToTextureFile.at(floorId);

    // Copy image to apply color scheme
    sf::Image image = m_mapTextures.at(tileSet).copyToImage();

    const sf::Vector2u size = image.getSize();
    const auto& [redBalance, greenBalance, blueBalance] = config::m_mapColorScheme.at(playerFloor);

    auto applySemitoneFilter = [](const int value, const int shift) -> int
    {
        return std::min(255, std::max(0, value + shift * 2)); // Simplified approximation for semitone effect
    };

    // Apply semitone filter for each pixel
    for (unsigned int x = 0; x < size.x; ++x)
        for (unsigned int y = 0; y < size.y; ++y)
        {
            sf::Color pixel = image.getPixel(x, y);

            const int r = applySemitoneFilter(pixel.r, redBalance);
            const int g = applySemitoneFilter(pixel.g, greenBalance);
            const int b = applySemitoneFilter(pixel.b, blueBalance);

            pixel.r = static_cast<sf::Uint8>(r);
            pixel.g = static_cast<sf::Uint8>(g);
            pixel.b = static_cast<sf::Uint8>(b);

            image.setPixel(x, y, pixel);
        }

    // Replace texture with applied filter
    m_mapTexturesWithColorSchemeApplied.at(tileSet).loadFromImage(image);

    // Change bg color
    std::string hexColor = config::colorToString(floorId);
    hexColor = hexColor.substr(1);

    int hexValue;
    std::stringstream ss;
    ss << std::hex << hexColor;
    ss >> hexValue;

    // Read rbg values and apply same filter
    int red = hexValue >> 16 & 0xFF;
    int green = hexValue >> 8 & 0xFF;
    int blue = hexValue & 0xFF;

    red = applySemitoneFilter(red, redBalance);
    green = applySemitoneFilter(green, greenBalance);
    blue = applySemitoneFilter(blue, blueBalance);

    std::stringstream stringHex;
    stringHex << "#" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << red << std::setw(2)
              << std::setfill('0') << std::hex << std::uppercase << green << std::setw(2) << std::setfill('0')
              << std::hex << std::uppercase << blue;

    m_currentBackgroundColor = stringHex.str();
}

sf::Color TextureSystem::getBackgroundColor() { return hexStringToSfmlColor(m_currentBackgroundColor); }
