#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "TextureAtlas.h"
#include "Utils/Helpers.cpp"

class MapSystem: public System{
   public:
    void draw(sf::RenderWindow& window) const;
    sf::Sprite createTile(uint32_t, sf::Vector2f, const float&, const sf::Vector2f&) const;
    void loadMap(std::string&);


    MapSystem(){
        texture_atlas.loadFromFile("../resources/FirstFloorAtlas/CosmicLilac.png");
        texture_atlas.loadFromFile("../resources/FirstFloorAtlas/Decorative.png");
        texture_atlas.loadFromFile("../resources/FirstFloorAtlas/DungeonWalls.png");
        texture_atlas.loadFromFile("../resources/FirstFloorAtlas/Graveyard.png");

    }

   private:
        TextureAtlas texture_atlas;
        static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);

        static std::string findKeyLessThan(
            const std::unordered_map<std::string, uint32_t>& atlas_sets, uint32_t i);
};
