#include "DealDMGToEnemySystem.h"

#include "CharacterComponent.h"
#include "Coordinator.h"
#include "DealDMGToEnemyEvent.h"
#include "MultiplayerComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void DealDMGToEnemySystem::update()
{
    for (const Entity entity : m_entities)
    {
        auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
        const auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        // Create damage indicator
        const Entity tag = gCoordinator.createEntity();
        gCoordinator.addComponent(tag, TextTagComponent{});
        gCoordinator.addComponent(tag, TransformComponent{enemyTransformComponent});

        // Deal dmg
        characterComponent.attacked = true;
        characterComponent.hp -= configSingleton.GetConfig().playerAttackDamage;
        applyKnockback(entity);
        m_entityToRemove.push(entity);
    }
    while (!m_entityToRemove.empty())
    {
        gCoordinator.removeComponent<DealDMGToEnemyEvent>(m_entityToRemove.top());
        m_entityToRemove.pop();
    }
}
void DealDMGToEnemySystem::applyKnockback(const Entity entityID) const
{
    auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(entityID);

    const auto& attackerPos = gCoordinator.getComponent<TransformComponent>(config::playerEntity).position;
    auto& targetPos = gCoordinator.getComponent<TransformComponent>(entityID).position;

    auto recoilDirection = targetPos - attackerPos;
    auto newRecoilDirection = normalize(glm::vec2{recoilDirection.x, recoilDirection.y});

    const float& recoilMagnitude = 20.0f;
    const auto recoilVelocity = recoilMagnitude * newRecoilDirection;

    enemyTransformComponent.velocity.x +=
        static_cast<float>(recoilVelocity.x * configSingleton.GetConfig().meterToPixelRatio);
    enemyTransformComponent.velocity.y +=
        static_cast<float>(recoilVelocity.y * configSingleton.GetConfig().meterToPixelRatio);
}