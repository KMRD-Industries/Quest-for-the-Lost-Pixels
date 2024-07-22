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
};
