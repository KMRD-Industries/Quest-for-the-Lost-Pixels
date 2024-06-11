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
    void incrementSpawnTimer();
    void processSpawner(Entity entity);
    bool isReadyToSpawn(int cooldown) const;
    static void spawnEnemy(Entity entity);
};
