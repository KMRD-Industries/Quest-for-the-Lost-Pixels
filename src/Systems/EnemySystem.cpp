#include "EnemySystem.h"
#include <random>
#include "TileComponent.h"
#include "TransformComponent.h"

void EnemySystem::update()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(-1, 1); // Uniform distribution between -1 and 1

    for (const auto& entity : m_entities)
    {
        if (gCoordinator.hasComponent<TransformComponent>(entity))
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            float rand_x = dis(gen); // Generate random number for x between -1 and 1
            float rand_y = dis(gen); // Generate random number for y between -1 and 1
            transformComponent.position.x += rand_x;
            transformComponent.position.y += rand_y;
        }
    }
}

Entity EnemySystem::getFirstUnused()
{
    for (const auto& entity : m_entities)
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