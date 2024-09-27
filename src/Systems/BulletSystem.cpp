
#include "BulletSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "ObjectCreatorSystem.h"
#include "RenderComponent.h"

extern Coordinator gCoordinator;

void BulletSystem::deleteBullets()
{
    for (const auto entity : m_entities)
    {
        gCoordinator.getComponent<CharacterComponent>(entity).hp = -1;
        gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
        gCoordinator.getComponent<RenderComponent>(entity).dirty = true;
    }
}

