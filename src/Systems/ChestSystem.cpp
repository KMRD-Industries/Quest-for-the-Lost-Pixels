#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "Physics.h"
//
// void ChestSystem::deleteItems()
// {
//     std::deque<Entity> entityToRemove;
//
//     for (const auto entity : m_entities)
//     {
//         entityToRemove.push_back(entity);
//     }
//
//     while (!entityToRemove.empty())
//     {
//         Physics::getWorld()->DestroyBody(gCoordinator.getComponent<ColliderComponent>(entityToRemove.front()).body);
//         gCoordinator.destroyEntity(entityToRemove.front());
//         entityToRemove.pop_front();
//     }
// }

void ChestSystem::deleteItems() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
        {
            gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
        }
        else
        {
            entitiesToKill.push_back(entity);
        }
    }

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}