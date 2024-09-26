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
    void update(const float timeStamp);
    void clearSpawners();
    void cleanUpUnnecessarySpawners();

    SpawnerSystem();

private:
    float m_spawnTime{0};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent);
    bool isReadyToSpawn(int cooldown);
    void spawnEnemy(const TransformComponent &, Enemies::EnemyType) const;
};
