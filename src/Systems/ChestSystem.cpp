#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Physics.h"

void ChestSystem::deleteItems()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        Physics::getWorld()->DestroyBody(gCoordinator.getComponent<ColliderComponent>(entityToRemove.front()).body);
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}