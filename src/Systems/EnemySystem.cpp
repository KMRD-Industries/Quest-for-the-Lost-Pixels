#include "EnemySystem.h"
#include <random>
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "Physics.h"
#include "TransformComponent.h"

EnemySystem::EnemySystem() { init(); }

void EnemySystem::init()
{
    gen.seed(rd()); // Seed the generator
    dis = std::uniform_real_distribution<float>(-1, 1); // Range between -1 and 1
}

void EnemySystem::update()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            // Generate random numbers and update velocity
            const float randX = dis(gen); // Generate random number for x between -1 and 1
            const float randY = dis(gen); // Generate random number for y between -1 and 1
            transformComponent.velocity.x += randX * config::enemyAcc;
            transformComponent.velocity.y += randY * config::enemyAcc;
        }
    }
}

void EnemySystem::deleteEnemies() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {
        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            colliderComponent->toDestroy = true;
        else
            entitiesToKill.push_back(entity);
    }

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}
