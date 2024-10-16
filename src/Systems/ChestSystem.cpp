#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "ItemComponent.h"
#include "WeaponComponent.h"

void ChestSystem::deleteItems() const
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<ItemComponent>(entity).equipped == true) continue;

        if (auto *collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
        {
            collisionComponent->toDestroy = true;
        }
    }
}
