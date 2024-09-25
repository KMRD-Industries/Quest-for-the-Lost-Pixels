#pragma once
#include "box2d/b2_math.h"

struct CharacterComponent
{
    float hp{1};
    float damage{};
    bool attacked{};
    int timeSinceAttacked{};
    b2Vec2 knockbackForce{};
};