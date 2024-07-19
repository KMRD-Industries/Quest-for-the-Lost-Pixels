#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"

struct TransformComponent
{
    sf::Vector2f position{0.f, 0.f};
    float rotation{0.f};
    sf::Vector2f scale{1.f, 1.f};
    b2Vec2 velocity{0.f, 0.f};

    TransformComponent() = default;

    TransformComponent(const sf::Vector2f& position, const float rotation, const sf::Vector2f scale,
                       const b2Vec2& velocity) :
        position{position}, rotation{rotation}, scale{scale}, velocity{velocity}
    {
    }

    explicit TransformComponent(const sf::Vector2f& position) : position{position} {}
};
