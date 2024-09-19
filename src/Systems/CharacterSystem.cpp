#include "CharacterSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

void CharacterSystem::update() { cleanUpDeadEntities(); }

void CharacterSystem::init() {}

void CharacterSystem::cleanUpDeadEntities() const
{
    std::unordered_set<Entity> entityToKill{};
    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<CharacterComponent>(entity).hp > 0) continue;
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
            gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
        else
            entityToKill.insert(entity);
    }
    for (const auto entity : entityToKill) gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}
