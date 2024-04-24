//
// Created by dominiq on 4/22/24.
//
#pragma once

#ifndef SF_MAPSYSTEM_H
#define SF_MAPSYSTEM_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "TextureAtlas.h"

class MapSystem: public System{
   public:
    void draw(sf::RenderWindow& window) const;
    sf::Sprite createTile(uint32_t, sf::Vector2f, float, float) const;

    MapSystem(){
        texture_atlas.loadFromFile("/home/dominiq/Desktop/KMDR/Quest-for-the-Lost-Pixels/resources/FirstFloorAtlas/CosmicLilac.png");
        texture_atlas.loadFromFile("/home/dominiq/Desktop/KMDR/Quest-for-the-Lost-Pixels/resources/FirstFloorAtlas/Decorative.png");
    }

   private:
        TextureAtlas texture_atlas;
};

#endif  // SF_MAPSYSTEM_H
