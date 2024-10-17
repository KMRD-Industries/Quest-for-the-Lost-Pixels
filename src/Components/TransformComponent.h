#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"
#include "glm/vec2.hpp"

struct TransformComponent
{
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f lastPosition{0.f, 0.f};
    sf::Vector2f scale{1.f, 1.f};
    float rotation{0};
    b2Vec2 velocity{0.f, 0.f};

    TransformComponent() = default;

    TransformComponent(const sf::Vector2f& position, const float rotation, const sf::Vector2f scale = {1.f, 1.f},
                       const b2Vec2& velocity = {0.f, 0.f}) :
        position{position}, scale{scale}, rotation{rotation}, velocity{velocity}
    {
    }

    explicit TransformComponent(const sf::Vector2f& position) : position{position} {}
    explicit TransformComponent(const glm::vec2& position) : position{position.x, position.y} {}
};