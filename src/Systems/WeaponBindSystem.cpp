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
            auto& weaponColliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
            weaponColliderComponent.trigger = true;
            weaponColliderComponent.tag = "Weapon";
        }
        else
        {
            auto weaponColliderComponent = ColliderComponent{.tag = "Weapon", .trigger = true};
            gCoordinator.addComponent(entity, weaponColliderComponent);
        }

        const Entity eventEntity = gCoordinator.createEntity();
        const auto weaponBindEvent = gCoordinator.getComponent<BindSwingWeaponEvent>(entity);

        const auto onCollisionEnterFunction = [this, entity](const GameType::CollisionData& data)
        {
            auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
            swingComponent.enemyColided.insert(data.entity);
            const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
            if (!weaponComponent.isAttacking) return;
            if (swingComponent.enemyHited.contains(data.entity)) return;

            swingComponent.enemyHited.insert(data.entity);
            gCoordinator.addComponent(data.entity, DealDMGToEnemyEvent{});
        };

        const auto onCollisionOutFunction = [this, entity](const GameType::CollisionData& data)
        {
            auto& swingComponent = gCoordinator.getComponent<WeaponSwingComponent>(entity);
            swingComponent.enemyColided.erase(data.entity);
            const auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(entity);
            if (!weaponComponent.isAttacking) return;
            if (swingComponent.enemyHited.contains(data.entity)) return;

            swingComponent.enemyHited.insert(data.entity);
            gCoordinator.addComponent(data.entity, DealDMGToEnemyEvent{});
        };

        const auto createNewCollisionEvent = CreateBodyWithCollisionEvent{
            .entity = entity,
            .tag = "Weapon",
            .onCollisionEnter = onCollisionEnterFunction, // collisions triggers functions for weapon handling
            .onCollisionOut = onCollisionOutFunction,
            .isStatic = false,
            .useTextureSize = false,
            .trigger = true};

        gCoordinator.addComponent(eventEntity, createNewCollisionEvent);
        m_entityToRemove.push(entity);
    }
    while (!m_entityToRemove.empty())
    {
        gCoordinator.removeComponent<BindSwingWeaponEvent>(m_entityToRemove.top());
        m_entityToRemove.pop();
    }
}
