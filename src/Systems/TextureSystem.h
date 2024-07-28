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
private:
    std::vector<std::string> texture_files{"CosmicLilac",   "Decorative", "DungeonWalls", "Jungle", "Graveyard",
                                           "SpecialBlocks", "AnimSlimes", "Characters",   "Weapons"};
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<long, sf::IntRect> texture_map;
    std::unordered_map<std::string, long> texture_indexes;
    std::unordered_map<long, std::vector<AnimationFrame>> map_animations;
    std::unordered_map<long, Collision> map_collisions;
    std::unordered_map<long, Collision> weapon_placements;

    long no_textures = 0;

public:
    int loadFromFile(const std::string&);
    void loadTexturesFromFiles();
    void loadTextures();

    sf::Sprite getTile(const std::string&, const long);
    std::vector<AnimationFrame> getAnimations(const std::string&, const long);
    Collision getCollision(const std::string& tileset_name, long id);
};
