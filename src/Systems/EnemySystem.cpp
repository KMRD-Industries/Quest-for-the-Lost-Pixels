#include "EnemySystem.h"
#include <random>
#include "Config.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void EnemySystem::update() const
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dis(-1, 1); // Uniform distribution between -1 and 1

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
