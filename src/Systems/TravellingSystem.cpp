#include "TravellingSystem.h"


#include "Coordinator.h"
#include "PassageComponent.h"
#include "TravellingDungeonComponent.h"

extern Coordinator gCoordinator;

void TravellingSystem::update()
{
    for (const auto entity : m_entities)
    {
        auto& travellingComponent = gCoordinator.getComponent<TravellingDungeonComponent>(entity);
        auto& movesInDungeon = travellingComponent.moveInDungeon;

        if (!movesInDungeon.empty())
        {
            const auto tmpMove = movesInDungeon.front();
            movesInDungeon.pop_front();
            travellingComponent.moveCallback(tmpMove);
        }
    }
}
