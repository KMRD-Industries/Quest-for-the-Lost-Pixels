#include "DoorSystem.h"
#include "Coordinator.h"
#include "DoorComponent.h"

extern Coordinator gCoordinator;

void DoorSystem::clearDoors()
{
    std::deque<Entity> entityToRemove;
    for (const auto& entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }
    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<DoorComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}
