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
    void clearSpawners() const;
    void cleanUpUnnecessarySpawners() const;

private:
    int spawnTime;
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent) const;
    bool isReadyToSpawn(int cooldown) const;
    static void spawnEnemy(const TransformComponent &,const Enemies::EnemyType enemyType);
};
