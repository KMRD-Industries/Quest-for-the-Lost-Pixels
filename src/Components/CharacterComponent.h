#pragma once
#include <functional>
#include <iostream>

#include "glm/vec2.hpp"

struct CharacterComponent
{
    float hp{100};
    bool attacked{};
    int timeSinceAttacked{};
};
