#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"

struct TransformComponent
{
    sf::Vector2f position{};
    float rotation{};
    sf::Vector2f scale{};
    b2Vec2 velocity{0.F, 0.F};
    TransformComponent() = default;

    TransformComponent(const sf::Vector2f& position, const float rotation, const sf::Vector2f scale,
                       const b2Vec2& velocity = {0.F, 0.F}) :
        position{position}, rotation{rotation}, scale{scale}, velocity{velocity}
    {
    }

    explicit TransformComponent(const sf::Vector2f& position) : position{position} {}
};
