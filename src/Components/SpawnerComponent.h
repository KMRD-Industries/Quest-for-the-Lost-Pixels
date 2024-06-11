#pragma once

struct SpawnerComponent
{
public:
    float spawnCooldown = {100};
    bool loopSpawn = false;
    int noSpawns = 0;
};