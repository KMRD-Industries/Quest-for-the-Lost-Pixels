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
    void update(float);
    void clearSpawners();
    void cleanUpUnnecessarySpawners();
    void spawnEnemies();
    std::vector<std::pair<Entity, sf::Vector2<float>>> getSortedSpawnedEnemies();

    SpawnerSystem();

private:
    float m_spawnTime{};
    std::vector<std::pair<Entity, sf::Vector2<float>>> m_spawnedEnemies{};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent) const;
    bool isReadyToSpawn(int cooldown);
    void spawnEnemy(const TransformComponent &, Enemies::EnemyType) const;
    void prepareEnemies();
};