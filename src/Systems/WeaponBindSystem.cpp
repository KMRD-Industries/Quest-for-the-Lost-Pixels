#include "WeaponBindSystem.h"

#include "BindSwingWeaponEvent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "DealDMGToEnemyEvent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

extern Coordinator gCoordinator;

void WeaponBindSystem::update()
{
    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
        {
            gCoordinator.getComponent<ColliderComponent>(entity).trigger = true;
            gCoordinator.getComponent<ColliderComponent>(entity).tag = "Weapon";
        }
        else
        {
            auto weaponColliderComponent = ColliderComponent{true, "Weapon"};
            gCoordinator.addComponent(entity, weaponColliderComponent);
        }

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
                gCoordinator.addComponent(data.entityID, DealDMGToEnemyEvent{});
            },
            [entity](const GameType::CollisionData& data)
            {
                auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
                swingComponent.enemyColided.erase(data.entityID);
                const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
                if (!weaponComponent.isAttacking) return;
                if (swingComponent.enemyHited.contains(data.entityID)) return;

                swingComponent.enemyHited.insert(data.entityID);
                gCoordinator.addComponent(data.entityID, DealDMGToEnemyEvent{});
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