#pragma once
#include "GameTypes.h"

struct SpawnerComponent
{
    float spawnCooldown = 100.f;
    bool loopSpawn = false;
    int noSpawns = 0;
    Enemies::EnemyType enemyType = Enemies::EnemyType::MELEE;
};