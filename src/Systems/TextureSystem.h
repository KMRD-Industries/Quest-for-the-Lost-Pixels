//
// Created by dominiq on 5/11/24.
//
#pragma once

#include <unordered_map>
#include <vector>
#include "AnimationComponent.h"
#include "AtlasComponents/Texture.h"
#include "System.h"

class TextureSystem : public System
{
private:
    std::vector<std::string> texture_files{"CosmicLilacTiles", "Decorative", "DungeonWalls",
                                           "Jungle",           "Graveyard",  "SpecialBlocks"};
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<long, sf::IntRect> texture_map;
    std::unordered_map<std::string, long> texture_indexes;
    std::unordered_map<long, std::vector<sf::Sprite>> map_animations;
    [[maybe_unused]] long no_textures = 0;

public:
    int loadFromFile(const std::string&);
    void loadTexturesFromFiles();
    [[maybe_unused]] sf::Sprite getTile(const std::string&, long);
    [[maybe_unused]] std::vector<sf::Sprite> getAnimations(const std::string&, long);
    void loadTextures();
};
