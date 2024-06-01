#pragma once

#include "CollisionSystem.h"
#include "GameTypes.h"
#include "Helpers.h"
#include "Map.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"

class MapSystem : public System
{
public:
    void loadMap(const std::string&);

private:
    sf::Vector2f size{};

    static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);
    static std::string findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i);
    void resetMap();
    static sf::Vector2f getPosition(int, int, int);

    enum FlipFlags : std::uint8_t
    {
        NoFlip = 0x0,
        VerticalFlip = 0x4, // 0100
        HorizontalFlip = 0x8, // 1000
        DiagonalFlip = 0x2, // 0010
        HorizontalVerticalFlip = 0xC, // 1100
        DiagonalVerticalFlip = 0x6, // 0110
        DiagonalHorizontalFlip = 0xA, // 1010
        AllFlips = 0xE // 1110
    };
};
