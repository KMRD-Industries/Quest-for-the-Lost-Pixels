#include "EnemySystem.h"
#include <random>
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "Physics.h"
#include "PublicConfigMenager.h"
#include "TransformComponent.h"

extern PublicConfigSingleton configSingleton;

EnemySystem::EnemySystem()
{
    gen.seed(rd()); // Seed the generator
    dis = std::uniform_real_distribution<float>(-1, 1); // Range between -1 and 1
}

void EnemySystem::update(std::unordered_map<Entity, sf::Vector2<float>> &enemies)
{
    for (const auto enemy : enemies)
    {
        if (gCoordinator.hasComponent<TransformComponent>(enemy.first))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(enemy.first);

            //TODO przsyłam pozycje serwera i odejmuję aktualną pozycję na kliencie
            transformComponent.velocity.x = enemy.second.x * 20;
            transformComponent.velocity.y = -enemy.second.y * 20;

            // printf("[Adding new positions] x: %f, y: %f\n", enemy.second.x, enemy.second.y);
        }
    }
    enemies.clear();
}

void EnemySystem::deleteEnemies() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            colliderComponent->toDestroy = true;
        else
            entitiesToKill.push_back(entity);

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}