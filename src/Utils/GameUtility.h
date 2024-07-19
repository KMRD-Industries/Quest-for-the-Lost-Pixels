#pragma once
#include "SFML/System/Vector2.hpp"

inline long mapWidth {};
inline long mapHeight {};
inline sf::Vector2f startingPosition {};
inline long floorId {0};

inline std::string colorToString()
{
    switch (floorId)
    {
        case 0: return "#331541";
        case 1: return "#18215d";
        case 2: return "#25392e";
        default: return "#17205C";
    }
}