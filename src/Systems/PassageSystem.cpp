#include "PassageSystem.h"

#include "Coordinator.h"
#include "PassageComponent.h"
#include "TravellingDungeonComponent.h"

extern Coordinator gCoordinator;

void PassageSystem::update()
{
    for (const auto entity : m_entities)
    {
         auto& passageComponent = gCoordinator.getComponent<PassageComponent>(entity);
         auto& movesInDungeon = passageComponent.moveInDungeon;

        if (!movesInDungeon.empty())
        {
            movesInDungeon.clear();
            passageComponent.moveCallback();
        }
    }
}
