#pragma once
#include <unordered_set>

#include "Types.h"


struct WeaponSwingComponent
{
    std::unordered_set<Entity> enemyHited;
    std::unordered_set<Entity> enemyColided;
    Entity playerEntity{};
};