#include "ChestSystem.h"

#include "AnimationSystem.h"
#include "CharacterComponent.h"

void ChestSystem::deleteItems()
{
    for (const auto entity : m_entities)
        gCoordinator.getComponent<CharacterComponent>(entity).hp = -1;
}