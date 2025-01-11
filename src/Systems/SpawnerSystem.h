#pragma once
#include "Coordinator.h"
#include "SpawnerComponent.h"
#include "System.h"
#include "TransformComponent.h"
#include "comm.pb.h"

extern Coordinator gCoordinator;

class SpawnerSystem : public System
{
public:
    SpawnerSystem() = default;

    void update(float);
    void clearSpawners();
    void spawnEnemies();
    std::vector<std::pair<Entity, sf::Vector2<float>>> getSortedSpawnedEnemies();
    void spawnOnDemand(const comm::StateUpdate &enemiesToSpawn) const;

private:
    float m_spawnTime{};
    std::vector<std::pair<Entity, sf::Vector2<float>>> m_spawnedEnemies{};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent) const;
    bool isReadyToSpawn(int cooldown);
    void cleanUpUnnecessarySpawners();
    void spawnEnemy(Entity newMonsterEntity, const comm::Enemy &enemyToSpawn) const;
    void prepareEnemies();
};