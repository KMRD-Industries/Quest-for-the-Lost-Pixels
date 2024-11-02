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
    void init();
    void update(float);
    void clearSpawners();
    void cleanUpUnnecessarySpawners();
    void spawnEnemies();
    std::vector<std::pair<Entity, sf::Vector2<float>>> getSortedSpawnedEnemies();
    void spawnOnDemand(const comm::EnemyPositionsUpdate &enemiesToSpawn) const;

    SpawnerSystem();

private:
    float m_spawnTime{};
    std::vector<std::pair<Entity, sf::Vector2<float>>> m_spawnedEnemies{};
    void incrementSpawnTimer();
    void processSpawner(SpawnerComponent &spawnerComponent, const TransformComponent &spawnerTransformComponent) const;
    bool isReadyToSpawn(int cooldown);
    Entity spawnEnemy(const comm::Enemy &enemyToSpawn) const;
    void prepareEnemies();
};