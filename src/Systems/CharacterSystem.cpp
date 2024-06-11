#include "CharacterSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

void CharacterSystem::update() const
{
    cleanUpDeadEntities();
}

void CharacterSystem::cleanUpDeadEntities() const
{
    std::unordered_set<Entity> entityToKill{};
    for (auto& entity : m_entities)
    {
        if (gCoordinator.getComponent<CharacterComponent>(entity).hp > 0)
            continue;
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
            gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
        else
            entityToKill.insert(entity);
    }
    for (auto& entity : entityToKill)
        gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}
