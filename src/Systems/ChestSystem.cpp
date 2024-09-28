#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "HelmetComponent.h"
#include "WeaponComponent.h"

void ChestSystem::deleteItems() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {
        if (const auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(entity))
            if (weaponComponent->equipped == true) continue;

        if (const auto* itemComponent = gCoordinator.tryGetComponent<HelmetComponent>(entity))
            if (itemComponent->equipped == true) continue;

        if (auto* collisionComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            collisionComponent->toDestroy = true;
    }
}