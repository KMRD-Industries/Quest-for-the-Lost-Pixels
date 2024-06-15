#pragma once

struct SpawnerComponent
{
public:
    float spawnCooldown = 100.f;
    bool loopSpawn = false;
    int noSpawns = 0;
};