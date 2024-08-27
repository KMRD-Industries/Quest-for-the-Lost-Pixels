#pragma once
#include "Coordinator.h"
#include "System.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    void update();
    void clearSpawners() const;
    void cleanUpUnnecessarySpawners();

private:
    int spawnTime;
    void incrementSpawnTimer();
    void processSpawner(Entity entity);
    bool isReadyToSpawn(const int cooldown) const;
    static void spawnEnemy(const Entity entity);
};
