#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipWeaponSystem.h"
#include "InventoryComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void InventorySystem::init(){};

void InventorySystem::update() {}

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

void InventorySystem::pickUpWeapon(const Entity player, const Entity weapon)
{
    if (gCoordinator.hasComponent<ColliderComponent>(weapon))
    {
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(weapon);
        if (colliderComponent.body != nullptr)
        {
            Physics::getWorld()->DestroyBody(colliderComponent.body);
            colliderComponent.body = nullptr;
            colliderComponent.collision = {};
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
