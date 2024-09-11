#pragma once
#include <functional>
#include <iostream>

#include "glm/vec2.hpp"

struct CharacterComponent
{
    float hp{};
    bool attacked{};
    int timeSinceAttacked{};
};
