#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"

struct TransformComponent
{
    sf::Vector2f position{};
    sf::Vector2f scale{};

    float rotation{};
    b2Vec2 velocity{0.f, 0.f};
    TransformComponent() = default;
    TransformComponent(const sf::Vector2f& position, const float rotation, const sf::Vector2f scale,
                       const b2Vec2& velocity = {0.f, 0.f}) :
        position{position}, scale{scale}, rotation{rotation}, velocity{velocity}
    {
    }

    explicit TransformComponent(const sf::Vector2f& position) : position{position} {}
};
