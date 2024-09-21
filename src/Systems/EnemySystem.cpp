#include "EnemySystem.h"
#include <random>
#include <stdint.h>
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "Physics.h"
#include "TransformComponent.h"

EnemySystem::EnemySystem() { init(); }

void EnemySystem::init()
{
    gen.seed(rd()); // Seed the generator
    dis = std::uniform_int_distribution(-1, 1); // Range between -1 and 1
}

void EnemySystem::update()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            // Generate random numbers and update velocity
            const int randX = dis(gen); // Generate random number for x between -1 and 1
            const int randY = dis(gen); // Generate random number for y between -1 and 1
            transformComponent.velocity.x += randX * config::enemyAcc;
            transformComponent.velocity.y += randY * config::enemyAcc;
        }
    }
}

void EnemySystem::deleteEnemies() const
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<EnemyComponent>(entity)) entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        Physics::getWorld()->DestroyBody(gCoordinator.getComponent<ColliderComponent>(entityToRemove.front()).body);
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}
