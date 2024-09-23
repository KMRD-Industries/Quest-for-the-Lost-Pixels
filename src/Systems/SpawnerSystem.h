#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    void init();
    void update();
    void clearSpawners();
    void cleanUpUnnecessarySpawners();

    SpawnerSystem();

private:
    int spawnTime{0};
    std::vector<std::pair<Collision, TileComponent>> enemiesDescription;
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent);
    bool isReadyToSpawn(int cooldown);
    void spawnEnemy(const TransformComponent &);
};
