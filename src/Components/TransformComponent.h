#pragma once

#include <SFML/System/Vector2.hpp>

struct TransformComponent {
    sf::Vector2f position;
    float rotation;
    sf::Vector2f scale;

    TransformComponent()
        : position(0.f, 0.f), rotation(0.f), scale(sf::Vector2f(1.f, 1.f))
    {
    }

    TransformComponent(sf::Vector2f position, float rotation, sf::Vector2f scale)
        : position(position), rotation(rotation), scale(scale)
    {
    }
};
