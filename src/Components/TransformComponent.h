#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"

struct TransformComponent
{
    sf::Vector2f position{};
    float rotation{};
    sf::Vector2f scale{};
    b2Vec2 velocity{0.f, 0.f};
    TransformComponent() = default;

    TransformComponent(sf::Vector2f position, float rotation, sf::Vector2f scale, b2Vec2 vecloity = {0.f, 0.f}) :
        position{position}, rotation{rotation}, scale{scale}, velocity{vecloity}
    {
    }

    TransformComponent(sf::Vector2f position) : position{position} {}
};
