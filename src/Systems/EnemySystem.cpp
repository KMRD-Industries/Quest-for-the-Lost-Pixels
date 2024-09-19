#include "EnemySystem.h"
#include <random>

#include "ColliderComponent.h"
#include "Config.h"
#include "EnemyComponent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void EnemySystem::update()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dis(-1, 1); // Uniform distribution between -1 and 1

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
