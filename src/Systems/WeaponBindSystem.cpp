#include "WeaponBindSystem.h"

#include "BindSwingWeaponEvent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

extern Coordinator gCoordinator;

void WeaponBindSystem::update()
{
    for (const auto entity : m_entities)
    {
        auto weaponColliderComponent = ColliderComponent{true, "Weapon"};
        gCoordinator.addComponent(entity, weaponColliderComponent);

        const Entity eventEntity = gCoordinator.createEntity();
        const auto newEvent = CreateBodyWithCollisionEvent(
            entity, "Weapon",
            [this, entity](const GameType::CollisionData& data)
            {
                auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
                swingComponent.enemyColided.insert(data.entityID);
                const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
                if (!weaponComponent.isAttacking) return;
                if (swingComponent.enemyHited.contains(data.entityID)) return;

                swingComponent.enemyHited.insert(data.entityID);
                auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(data.entityID);
                const auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(data.entityID);

                // Create damage indicator
                const Entity tag = gCoordinator.createEntity();
                gCoordinator.addComponent(tag, TextTagComponent{});
                gCoordinator.addComponent(tag, TransformComponent{enemyTransformComponent});

                // Deal dmg
                characterComponent.attacked = true;
                characterComponent.hp -= configSingleton.GetConfig().playerAttackDamage;
                applyKnockback(entity);
            },
            [entity](const GameType::CollisionData& data)
            {
                auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
                swingComponent.enemyColided.erase(data.entityID);
            },
            false, false, true);

        gCoordinator.addComponent(eventEntity, newEvent);
        m_entityToRemove.push(entity);
    }
    while (!m_entityToRemove.empty())
    {
        gCoordinator.removeComponent<BindSwingWeaponEvent>(m_entityToRemove.top());
        m_entityToRemove.pop();
    }
}

void WeaponBindSystem::applyKnockback(const Entity entityID) const
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

    auto newPosition = targetPos + 0.25;
    newPosition = {newPosition.x * newRecoilDirection.x, newPosition.y * newRecoilDirection.y};
    targetPos = newPosition;
}