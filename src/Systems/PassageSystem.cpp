#include "PassageSystem.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "PassageComponent.h"
#include "Physics.h"
#include "PlayerComponent.h"

extern Coordinator gCoordinator;

void PassageSystem::update()
{
    for (const auto entity : m_entities)
    {
        if (auto& [moveInDungeon, moveCallback, activePassage] = gCoordinator.getComponent<PassageComponent>(entity);
            !moveInDungeon.empty())
        {
            moveInDungeon.clear();
            moveCallback();
        }
    }
}

void PassageSystem::setPassages(const bool val) const
{
    for (const auto entity : m_entities)
    {
        gCoordinator.getComponent<PassageComponent>(entity).activePassage = val;

        if (val == true && !gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            const Entity newMapCollisionEntity = gCoordinator.createEntity();
            const auto createNewCollisionEvent = CreateBodyWithCollisionEvent{
                .entity = entity, .tag = "Passage", .isStatic = true, .useTextureSize = true};

            gCoordinator.addComponent(newMapCollisionEntity, createNewCollisionEvent);
        }
    }
}


void PassageSystem::clearPassages() const
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entityToRemove.front());
        if (colliderComponent.body != nullptr) Physics::getWorld()->DestroyBody(colliderComponent.body);
        gCoordinator.removeComponent<PassageComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}
