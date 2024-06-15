#include "EnemySystem.h"
#include <random>
#include "Config.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void EnemySystem::update()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(-1, 1); // Uniform distribution between -1 and 1

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            float randX = dis(gen); // Generate random number for x between -1 and 1
            float randY = dis(gen); // Generate random number for y between -1 and 1
            transformComponent.velocity.x += randX * config::enemyAcc;
            transformComponent.velocity.y += randY * config::enemyAcc;
        }
    }
}

Entity EnemySystem::getFirstUnused()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<TileComponent>(entity))
        {
            auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
            if (tileComponent.id == 0)
            {
                return entity;
            }
        }
    }
    return {}; // Return a null entity if none found
}
void EnemySystem::deleteEnemies()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<TileComponent>(entity))
        {
            auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

            tileComponent = {};
            transformComponent = {};
            renderComponent = {};
        }
    }
}
