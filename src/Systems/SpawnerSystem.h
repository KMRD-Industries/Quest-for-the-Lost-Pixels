#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    void update();
    void clearSpawners();
    void cleanUpUnnecessarySpawners();

private:
    int spawnTime{0};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent);
    bool isReadyToSpawn(int cooldown);
    void spawnEnemy(const TransformComponent &);
};
