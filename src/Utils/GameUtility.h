#pragma once
#include "SFML/System/Vector2.hpp"
#include "glm/vec2.hpp"

namespace GameUtility
{
    inline long mapWidth{};
    inline long mapHeight{};
    inline sf::Vector2f mapOffset{0.f, 0.f};
    inline glm::vec2 startingPosition{325.f, 325.f};
} // namespace GameUtility
