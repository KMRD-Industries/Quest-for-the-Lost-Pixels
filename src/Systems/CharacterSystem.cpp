#include "CharacterSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "PublicConfigMenager.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

void CharacterSystem::update(const float& deltaTime)
{
    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * 1000)
    {
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * 1000;
        performFixedUpdate();
    }
}

void CharacterSystem::performFixedUpdate() { cleanUpDeadEntities(); }


void CharacterSystem::cleanUpDeadEntities() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {
        const auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
        if (characterComponent.hp > 0) continue;

        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
        {
            colliderComponent->toDestroy = true;
        }
        else
        {
            entitiesToKill.push_back(entity);
        }
    }

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}