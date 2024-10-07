#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "BodyArmourComponent.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipmentComponent.h"
#include "HelmetComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void InventorySystem::dropItem(const Entity player, const Entity item, const config::slotType slot) const {
    auto &equipmentComponent = gCoordinator.getComponent<EquipmentComponent>(player);
    const Entity newItemEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(
        newItemEntity,
        TileComponent{gCoordinator.getComponent<TileComponent>(item)},
        TransformComponent{gCoordinator.getComponent<TransformComponent>(item)},
        RenderComponent{},
        ColliderComponent{},
        AnimationComponent{},
        ItemComponent{},
        ItemAnimationComponent{
            .animationDuration = 1,
            .startingPositionY = gCoordinator.getComponent<TransformComponent>(player).position.y - 75
        });

    gCoordinator.destroyEntity(equipmentComponent.slots.at(slot));

    switch (slot) {
        case config::slotType::WEAPON: {
            gCoordinator.addComponent(newItemEntity, WeaponComponent{gCoordinator.getComponent<WeaponComponent>(item)});
            break;
        }
        case config::slotType::HELMET: {
            gCoordinator.addComponent(newItemEntity, HelmetComponent{gCoordinator.getComponent<HelmetComponent>(item)});
            break;
        }
        case config::slotType::BODY_ARMOUR: {
            gCoordinator.addComponent(newItemEntity,
                                      BodyArmourComponent{gCoordinator.getComponent<BodyArmourComponent>(item)});
            break;
        }
    }

    equipmentComponent.slots.erase(slot);
}

void InventorySystem::pickUpItem(const Entity player, const Entity item, const config::slotType type) const {
    if (const auto *colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(item))
        if (colliderComponent->body != nullptr)
            Physics::getWorld()->DestroyBody(colliderComponent->body);

    gCoordinator.removeComponentIfExists<ItemAnimationComponent>(item);
    gCoordinator.getComponent<ItemComponent>(item).equipped = true;

    auto &[equipment] = gCoordinator.getComponent<EquipmentComponent>(player);

    if (const auto it = equipment.find(type); it != equipment.end()) {
        dropItem(player, equipment.at(type), type);
    }

    equipment.emplace(type, item);
}
