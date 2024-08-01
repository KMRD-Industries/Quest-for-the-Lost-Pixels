#pragma once
#include <functional>
#include <iostream>

#include "glm/vec2.hpp"

struct CharacterComponent
{
    float hp{};
    bool attacked;

    CharacterComponent() : hp(100.0f), attacked(false) {}
};
