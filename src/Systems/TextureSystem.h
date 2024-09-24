//
// Created by dominiq on 5/11/24.
//
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
    std::unordered_set<std::string> m_setTextureFiles{"CosmicLilac", "Decorative",    "DungeonWalls", "Jungle",
                                                      "Graveyard",   "SpecialBlocks", "AnimSlimes",   "Characters",
                                                      "Weapons",     "CosmicLegacy",  "Portal"};

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
    TextureSystem() { init(); }
    int loadFromFile(const std::string &);
    long initializeTileSet(const Tileset &);
    void loadTexturesFromFiles();
    void loadTextures();
    void init();
    void update();

    sf::Sprite getTile(const std::string &, long) const;
    std::vector<AnimationFrame> getAnimations(const std::string &, long);
    Collision getCollision(const std::string &, long);
    void modifyColorScheme(int);
    std::string getBackgroundColor();
};
