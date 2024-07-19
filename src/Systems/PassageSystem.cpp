#include "PassageSystem.h"

#include "Coordinator.h"
#include "PassageComponent.h"
#include "TravellingDungeonComponent.h"

extern Coordinator gCoordinator;

void PassageSystem::update() const
{
    for (const auto entity : m_entities)
    {
        if (auto& [moveInDungeon, moveCallback] = gCoordinator.getComponent<PassageComponent>(entity);
            !moveInDungeon.empty())
        {
            moveInDungeon.clear();
            moveCallback();
        }
    }
}
