#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "ItemComponent.h"

void ChestSystem::deleteItems() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {

        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
        {
            colliderComponent->toDestroy = true;
        }
        else
        {
            entitiesToKill.push_back(entity);
        }
    }

    for (const auto entity : entitiesToKill)
    {
        gCoordinator.removeComponent<ItemComponent>(entity);
    }
}