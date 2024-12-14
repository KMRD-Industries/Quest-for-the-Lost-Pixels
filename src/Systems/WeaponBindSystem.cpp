#include "WeaponBindSystem.h"

#include "BindSwingWeaponEvent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "DealDMGToEnemyEvent.h"
#include "MultiplayerSystem.h"
#include "SynchronisedEvent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

extern Coordinator gCoordinator;

void WeaponBindSystem::update()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
        {
            auto& weaponColliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
            weaponColliderComponent.trigger = true;
            weaponColliderComponent.tag = "Weapon";
        }
        else
        {
            auto weaponColliderComponent = ColliderComponent{true, "Weapon"};
            gCoordinator.addComponent(entity, weaponColliderComponent);
        }

        const Entity playerEntity = gCoordinator.getComponent<BindSwingWeaponEvent>(entity).playerEntity;
        const Entity eventEntity = gCoordinator.createEntity();
        const auto newEvent = CreateBodyWithCollisionEvent(
            entity, "Weapon",
            [this, entity](const GameType::CollisionData& data)
            {
                auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
                swingComponent.enemyColided.insert(data.entity);
                const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
                if (!weaponComponent.isAttacking) return;
                if (swingComponent.enemyHited.contains(data.entity)) return;

                swingComponent.enemyHited.insert(data.entity);
                gCoordinator.addComponent(data.entity, DealDMGToEnemyEvent{});
            },
            [entity, playerEntity](const GameType::CollisionData& data)
            {
                auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
                swingComponent.enemyColided.erase(data.entity);
                const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
                if (!weaponComponent.isAttacking) return;
                if (swingComponent.enemyHited.contains(data.entity)) return;

                swingComponent.enemyHited.insert(data.entity);
                gCoordinator.addComponent(data.entity, DealDMGToEnemyEvent{.playerEntity = playerEntity});
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
