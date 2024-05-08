#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "TextureAtlas.h"
#include "Utils/Helpers.cpp"

class MapSystem: public System{
   public:
    void draw(sf::RenderWindow& window) const;
    sf::Sprite createTile(uint32_t, const sf::Vector2f&, const float&, const sf::Vector2f&) const;
    void loadMap(std::string&);

    MapSystem(){
        texture_atlas.loadFromFile("../../resources/TileSets/CosmicLilacTiles.json");
        texture_atlas.loadFromFile("../../resources/TileSets/AnimSlimes.json");
        texture_atlas.loadFromFile("../../resources/TileSets/Decorative.json");
        texture_atlas.loadFromFile("../../resources/TileSets/Jungle.json");
        texture_atlas.loadFromFile("../../resources/TileSets/DungeonWalls.json");
        texture_atlas.loadFromFile("../../resources/TileSets/EnchantedForestAnimation.json");
        texture_atlas.loadFromFile("../../resources/TileSets/Graveyard.json");
        texture_atlas.loadFromFile("../../resources/TileSets/RetroAdventure.json");
    }

   private:
        TextureAtlas texture_atlas;
        sf::Vector2f size;
        static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);

        static std::string findKeyLessThan(
            const std::unordered_map<std::string, uint32_t>& atlas_sets, uint32_t i);
};
