#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    void update();

private:
    int spawnTime;
    void updateSpawnTime();
    void updateSpawner(Entity entity);
    bool isTimeForNextEnemy(int cooldown);
    void spawnEnemy(Entity entity);
};
