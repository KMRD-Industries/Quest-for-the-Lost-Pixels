#pragma once

#include <SFML/System/Vector2.hpp>
#include "box2d/b2_math.h"
#include "glm/vec2.hpp"

struct TransformComponent
{
    sf::Vector2f position{0.f, 0.f};
    sf::Vector2f scale{1.f, 1.f};
    sf::Vector2f centerOfMass{0.f, 0.f};
    float rotation{0};
    b2Vec2 velocity{0.f, 0.f};
};