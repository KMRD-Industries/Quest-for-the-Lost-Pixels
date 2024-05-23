#pragma once

#include <unordered_map>
#include <vector>
#include "AnimationComponent.h"
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"
#include "System.h"

class TextureSystem : public System
{
private:
    std::vector<std::string> texture_files{"CosmicLilacTiles", "Decorative",    "DungeonWalls", "Jungle",
                                           "Graveyard",        "SpecialBlocks", "AnimSlimes",   "Characters"};

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<long, sf::IntRect> textureMap;
    std::unordered_map<std::string, long> textureIndexes;
    std::unordered_map<long, std::vector<AnimationFrame>> mapAnimations;
    long noTextures = 0;

public:
    int loadFromFile(const std::string&);
    void loadTexturesFromFiles();
    sf::Sprite getTile(const std::string&, long);
    std::vector<AnimationFrame> getAnimations(const std::string&, long);
    void loadTextures();
};
