#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    SpawnerSystem() = default;

    void update(float);
    void clearSpawners();
    void spawnEnemies();

private:
    float m_spawnTime{};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent) const;
    bool isReadyToSpawn(int cooldown);
    void spawnEnemy(const TransformComponent &, Enemies::EnemyType) const;
    void cleanUpUnnecessarySpawners();
};