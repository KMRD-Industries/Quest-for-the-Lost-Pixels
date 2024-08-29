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
private:
    std::vector<std::string> m_vecTextureFiles{"CosmicLilac", "Decorative",    "DungeonWalls", "Jungle",
                                               "Graveyard",   "SpecialBlocks", "AnimSlimes",   "Characters",
                                               "Weapons",     "CosmicLegacy",  "Portal"};

    std::unordered_map<int, std::string> m_mapFloorToTextureFile{{1, "CosmicLilac"}, {2, "Jungle"}};
    std::unordered_map<long, long> m_mapDungeonLevelToFloorInfo{{1, 1}, {2, 1}, {3, 1}, {4, 2}, {5, 2}};

    struct ColorBalance
    {
        int redBalance{0};
        int greenBalance{0};
        int blueBalance{0};
    };

    std::unordered_map<long, ColorBalance> m_mapColorScheme{
        {1, {25, 0, 0}}, {2, {0, 25, 0}}, {3, {0, 15, 35}}, {4, {45, 6, 35}}, {5, {15, 62, 35}},
    };

    std::unordered_map<std::string, sf::Texture> m_mapTextures;
    std::unordered_map<long, sf::IntRect> m_mapTextureRects;
    std::unordered_map<std::string, long> m_mapTextureIndexes;
    std::unordered_map<long, std::vector<AnimationFrame>> m_mapAnimations;
    std::unordered_map<long, Collision> m_mapCollisions;
    long m_lNoTextures = 0;

public:
    int loadFromFile(const std::string&);
    void loadTexturesFromFiles();
    void loadTextures();

    sf::Sprite getTile(const std::string&, long);
    std::vector<AnimationFrame> getAnimations(const std::string&, long);
    Collision getCollision(const std::string& tileset_name, long id);
    void modifyColorScheme(int);
};
