//
// Created by dominiq on 4/22/24.
//

#ifndef SF_TRANSFORMCOMPONENT_H
#define SF_TRANSFORMCOMPONENT_H

#endif  // SF_TRANSFORMCOMPONENT_H


#pragma once
#include <SFML/System/Vector2.hpp>

struct TransformComponent {
    sf::Vector2f position;
};

struct TransformMapComponent {
    sf::Vector2f position;
    float rotation;
    sf::Vector2f scale;

    TransformMapComponent()
        : position(0.f, 0.f), rotation(0.f), scale(sf::Vector2f(1.f, 1.f))
    {
    }

    TransformMapComponent(sf::Vector2f position, float rotation, sf::Vector2f scale)
    : position(position), rotation(rotation), scale(scale)
    {
    }
};