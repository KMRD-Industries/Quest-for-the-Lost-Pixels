#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"
#include "GameTypes.h"
#include "System.h"
#include "Tileset.h"

class TextureSystem : public System
{
public:
    TextureSystem();

    void init();
    void update();
    void loadTextures();

    sf::Sprite getTile(const std::string& tilesetName, long tileIndex) const;
    std::vector<AnimationFrame> getAnimations(const std::string& tilesetName, long tileIndex);
    Collision getCollision(const std::string& tilesetName, long tileIndex);
    sf::Color getBackgroundColor();

    void modifyColorScheme(int colorSchemeIndex);

    std::vector<std::pair<uint32_t, GameType::WeaponType>> m_weaponsIDs;
    std::vector<uint32_t> m_helmets;
    std::vector<uint32_t> m_bodyArmours;

private:
    std::unordered_set<std::string> m_setTextureFiles{
        "Items",      "CosmicLilac", "Decorative", "DungeonWalls", "Jungle", "Graveyard", "SpecialBlocks",
        "AnimSlimes", "Characters",  "Weapons",    "CosmicLegacy", "Portal", "Armour"};

    std::unordered_map<std::string, sf::Texture> m_mapTextures;
    std::unordered_map<std::string, sf::Texture> m_mapTexturesWithColorSchemeApplied;

    std::unordered_map<long, sf::IntRect> m_mapTextureRects;
    std::unordered_map<std::string, long> m_mapTextureIndexes;
    std::unordered_map<long, std::vector<AnimationFrame>> m_mapAnimations;
    std::unordered_map<long, Collision> m_mapCollisions;
    std::unordered_map<long, Collision> m_mapWeaponPlacements;
    std::unordered_map<long, Collision> m_mapHelmetPlacements;
    std::unordered_map<long, Collision> m_mapBodyArmourPlacement;

    std::string m_currentBackgroundColor = "#331541";
    long m_lNoTextures = 0;

    void loadTilesIntoSystem(const Tileset&, long& firstGid);
    void loadAnimationsAndCollisionsIntoSystem(const Tileset&, const long& firstGid);
    long initializeTileSet(const Tileset&);
    int loadFromFile(const std::string& filename);
    void loadTexturesFromFiles();
};
