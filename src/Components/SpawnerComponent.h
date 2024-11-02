#pragma once
#include "GameTypes.h"

struct SpawnerComponent
{
    //TODO 1. zrób tak żeby spawner też miał pozycję, bo aktualnie jest robione obejście żeby przesłać spawnery na srwer
    //TODO 2. serwer musi przesłać jaki typ serwera stworzyć więc to serwer musi (w przyszłości decydować co stworzyć)
    float spawnCooldown = 100.f;
    bool loopSpawn = false;
    int noSpawns = 0;
    Enemies::EnemyType enemyType = Enemies::EnemyType::MELEE;
};