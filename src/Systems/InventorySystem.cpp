#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipWeaponSystem.h"
#include "EquippedHelmetComponent.h"
#include "EquippedWeaponComponent.h"
#include "HelmetComponent.h"
#include "InventoryComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void InventorySystem::dropWeapon(const Entity player)
{
    auto& inventory = gCoordinator.getComponent<InventoryComponent>(player);
    gCoordinator.getComponent<WeaponComponent>(inventory.weapons[0]).equipped = false;

    if (!gCoordinator.hasComponent<ItemComponent>(inventory.weapons[0]))
        gCoordinator.addComponent(inventory.weapons[0], ItemComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();
    gCoordinator.getComponent<TransformComponent>(inventory.weapons[0]).rotation = 0;

    const auto newEvent = CreateBodyWithCollisionEvent(
        inventory.weapons[0], "Item", [this](const GameType::CollisionData&) {}, [&](const GameType::CollisionData&) {},
        false, false);

    gCoordinator.addComponent(newItemEntity, newEvent);
    inventory.weapons.clear();
}

void InventorySystem::dropItem(const Entity player)
{
    auto& inventory = gCoordinator.getComponent<InventoryComponent>(player);
    gCoordinator.getComponent<HelmetComponent>(inventory.helmets[0]).equipped = false;

    if (!gCoordinator.hasComponent<ItemComponent>(inventory.helmets[0]))
        gCoordinator.addComponent(inventory.helmets[0], ItemComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();
    gCoordinator.getComponent<TransformComponent>(inventory.helmets[0]).rotation = 0;

    const auto newEvent = CreateBodyWithCollisionEvent(
        inventory.helmets[0], "Item", [this](const GameType::CollisionData&) {}, [&](const GameType::CollisionData&) {},
        false, false);

    gCoordinator.addComponent(newItemEntity, newEvent);
    inventory.helmets.clear();
}


void InventorySystem::pickUpItem(const Entity player, const Entity helmet)
{
    if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(helmet))
    {
        if (colliderComponent->body != nullptr)
        {
            Physics::getWorld()->DestroyBody(colliderComponent->body);
        }
    }

    if (const auto* equippedHelmetComponent = gCoordinator.tryGetComponent<EquippedHelmetComponent>(player))
    {
        if (gCoordinator.hasComponent<TransformComponent>(equippedHelmetComponent->currentHelmet))
        {
            const auto& equippedHelmetCollisionComponent =
                gCoordinator.getComponent<ColliderComponent>(equippedHelmetComponent->currentHelmet);
            const auto& newWeaponCollisionComponent = gCoordinator.getComponent<ColliderComponent>(helmet);

            auto& equippedTransformComponent =
                gCoordinator.getComponent<TransformComponent>(equippedHelmetComponent->currentHelmet);

            equippedTransformComponent.angle = 0;
            equippedTransformComponent.rotation = 0;

            equippedTransformComponent = gCoordinator.getComponent<TransformComponent>(helmet);

            equippedTransformComponent.position.y +=
                (newWeaponCollisionComponent.collision.height - equippedHelmetCollisionComponent.collision.height) *
                config::gameScale;

            dropItem(player);
        }
    }

    auto& inventory = gCoordinator.getComponent<InventoryComponent>(player);
    inventory.helmets.push_back(helmet);
    gCoordinator.getComponent<HelmetComponent>(helmet).equipped = true;

    if (gCoordinator.hasComponent<ItemComponent>(helmet)) gCoordinator.removeComponent<ItemComponent>(helmet);

    if (gCoordinator.hasComponent<ItemAnimationComponent>(helmet))
        gCoordinator.removeComponent<ItemAnimationComponent>(helmet);

    gCoordinator.getRegisterSystem<EquipWeaponSystem>()->equipHelmet(player, helmet);
}

void InventorySystem::pickUpWeapon(const Entity player, const Entity weapon)
{
    if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(weapon))
    {
        if (colliderComponent->body != nullptr)
        {
            Physics::getWorld()->DestroyBody(colliderComponent->body);
        }
    }

    if (const auto* equippedWeaponComponent = gCoordinator.tryGetComponent<EquippedWeaponComponent>(player))
    {
        if (gCoordinator.hasComponent<TransformComponent>(equippedWeaponComponent->currentWeapon))
        {
            const auto& equippedWeaponCollisionComponent =
                gCoordinator.getComponent<ColliderComponent>(equippedWeaponComponent->currentWeapon);
            const auto& newWeaponCollisionComponent = gCoordinator.getComponent<ColliderComponent>(weapon);

            auto& equippedTransformComponent =
                gCoordinator.getComponent<TransformComponent>(equippedWeaponComponent->currentWeapon);

            equippedTransformComponent = gCoordinator.getComponent<TransformComponent>(weapon);

            equippedTransformComponent.position.y +=
                (newWeaponCollisionComponent.collision.height - equippedWeaponCollisionComponent.collision.height) *
                config::gameScale;

            dropWeapon(player);
        }
    }

    auto& inventory = gCoordinator.getComponent<InventoryComponent>(player);
    inventory.weapons.push_back(weapon);
    gCoordinator.getComponent<WeaponComponent>(weapon).equipped = true;

    if (gCoordinator.hasComponent<ItemComponent>(weapon)) gCoordinator.removeComponent<ItemComponent>(weapon);

    if (gCoordinator.hasComponent<ItemAnimationComponent>(weapon))
        gCoordinator.removeComponent<ItemAnimationComponent>(weapon);

    gCoordinator.getRegisterSystem<EquipWeaponSystem>()->equipWeapon(player, weapon);
}
