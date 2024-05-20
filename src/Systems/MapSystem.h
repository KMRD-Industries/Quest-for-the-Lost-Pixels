#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "GameTypes.h"
#include "Helpers.h"

class MapSystem : public System
{
public:
    void loadMap(std::string&);

private:
    sf::Vector2f size{};

    static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);
    static std::string findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i);
};
