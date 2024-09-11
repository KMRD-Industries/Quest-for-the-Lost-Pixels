#pragma once

#include <unordered_map>
#include <vector>
#include "AnimationComponent.h"
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"
#include "System.h"
#include "Tileset.h"

class TextureSystem : public System
{
    std::vector<std::string> m_vecTextureFiles{"CosmicLilac", "Decorative",    "DungeonWalls", "Jungle",
                                               "Graveyard",   "SpecialBlocks", "AnimSlimes",   "Characters",
                                               "Weapons",     "CosmicLegacy",  "Portal"};

    //------------------------------------------
    // Nie mam jeszcze pomysłu gdzie to dać

    struct ColorBalance
    {
        int redBalance{0};
        int greenBalance{0};
        int blueBalance{0};
    };

    static std::string colorToString(const int floorID)
    {
        switch (floorID)
        {
        case 0:
            return "#331541";
        case 1:
            return "#18215d";
        case 2:
            return "#25392e";
        default:
            return "#17205C";
        }
    }

    std::unordered_map<int, std::string> m_mapFloorToTextureFile{{1, "CosmicLilac"}, {2, "Jungle"}};
    std::unordered_map<long, long> m_mapDungeonLevelToFloorInfo{{1, 1}, {2, 1}, {3, 1}, {4, 2}, {5, 2}};
    std::unordered_map<long, ColorBalance> m_mapColorScheme{
        {1, {25, 0, 0}}, {2, {0, 25, 0}}, {3, {0, 15, 35}}, {4, {45, 6, 35}}, {5, {15, 62, 35}},
    };

    //------------------------------------------

    std::unordered_map<std::string, sf::Texture> m_mapTextures;
    std::unordered_map<std::string, sf::Texture> m_mapTexturesWithColorSchemeApplied;

    std::unordered_map<long, sf::IntRect> m_mapTextureRects;
    std::unordered_map<std::string, long> m_mapTextureIndexes;
    std::unordered_map<long, std::vector<AnimationFrame>> m_mapAnimations;
    std::unordered_map<long, Collision> m_mapCollisions;
    std::string m_currentBackgroundColor = "#331541";
    std::unordered_map<long, Collision> m_mapWeaponPlacements;
    long m_lNoTextures = 0;

    void loadTilesIntoSystem(const Tileset &, long &);
    void loadAnimationsAndCollisionsIntoSystem(const Tileset &, const long &firstGid);

public:
    int loadFromFile(const std::string &);
    long initializeTileSet(const Tileset &);
    void loadTexturesFromFiles();
    void loadTextures();

    sf::Sprite getTile(const std::string &, long) const;
    std::vector<AnimationFrame> getAnimations(const std::string &, long);
    Collision getCollision(const std::string &, long);
    void modifyColorScheme(int);
    std::string getBackgroundColor();
};
