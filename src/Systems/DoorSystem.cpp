#include "DoorSystem.h"
#include "Coordinator.h"
#include "DoorComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void DoorSystem::clearDoors() const
{
    std::deque<Entity> entityToRemove;
    for (const auto entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }
    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<DoorComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}

sf::Vector2f DoorSystem::getDoorPosition(const GameType::DoorEntraces entrance) const
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.getComponent<DoorComponent>(entity).entrance == entrance)
            return gCoordinator.getComponent<TransformComponent>(entity).position;
    }
    return {};
}
