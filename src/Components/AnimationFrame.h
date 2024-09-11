#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

struct AnimationFrame
{
    long tileid{};
    long duration{};
    float rotation{};
};
