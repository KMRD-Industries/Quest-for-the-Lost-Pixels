#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "TextureAtlas.h"
#include "Utils/Helpers.cpp"

class MapSystem : public System
{
public:
    void draw(sf::RenderWindow& window) const;
    void loadMap(std::string&);

private:
    sf::Vector2f size;
    static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);
    static std::string findKeyLessThan(const std::unordered_map<std::string, uint32_t>& atlas_sets, uint32_t i);
};
