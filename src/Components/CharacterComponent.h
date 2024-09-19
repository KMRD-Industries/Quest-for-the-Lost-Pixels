#pragma once
#include "box2d/b2_math.h"

struct CharacterComponent
{
    float hp{};
    bool attacked{};
    int timeSinceAttacked{};
    b2Vec2 knockbackForce{};
};