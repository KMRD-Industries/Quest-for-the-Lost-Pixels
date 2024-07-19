#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    void update();
    void clearSpawners() const;

private:
    int spawnTime;
    void incrementSpawnTimer();
    void processSpawner(const Entity entity) const;
    bool isReadyToSpawn(const int cooldown) const;
    static void spawnEnemy(const Entity entity);
};
