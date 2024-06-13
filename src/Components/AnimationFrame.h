#pragma once
#include <SFML/System/Vector2.hpp>
#include <cstdint>

struct AnimationFrame
{
    int64_t tileid{};
    int64_t duration{};
};