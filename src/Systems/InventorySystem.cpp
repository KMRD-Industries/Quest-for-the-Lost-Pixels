#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipmentComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "TransformComponent.h"

void InventorySystem::dropItem(const Entity player, const Entity item, const config::slotType slot) const {
    auto &equipmentComponent = gCoordinator.getComponent<EquipmentComponent>(player);

    if (!gCoordinator.hasComponent<ItemComponent>(item))
        gCoordinator.addComponent(item, ItemComponent{.equipped = false});

    gCoordinator.getComponent<ItemComponent>(item).equipped = false;

    //TODO: Dropping item position logic
    equipmentComponent.slots.erase(slot);

    const auto newItem = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(
        item, "Item", [this](const GameType::CollisionData &) {
        }, [&](const GameType::CollisionData &) {
        },
        false, false);

    gCoordinator.addComponent(newItem, newEvent);
}

void InventorySystem::pickUpItem(const Entity player, const Entity item, const config::slotType type) const {
    gCoordinator.removeComponentIfExists<ItemAnimationComponent>(item);
    gCoordinator.getComponent<ItemComponent>(item).equipped = true;

    auto &[equipment] = gCoordinator.getComponent<EquipmentComponent>(player);

    if (const auto it = equipment.find(type); it != equipment.end()) {
        // Adjust dropped body position
        if (const auto *colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(item)) {
            if (colliderComponent->body != nullptr) {
                gCoordinator.getComponent<TransformComponent>(equipment.at(type)).position = {
                    convertMetersToPixel(colliderComponent->body->GetPosition().x),
                    convertMetersToPixel(colliderComponent->body->GetPosition().y)
                };
            }
        }

        dropItem(player, equipment.at(type), type);
    }

    if (const auto *colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(item))
        if (colliderComponent->body != nullptr)
            Physics::getWorld()->DestroyBody(colliderComponent->body);


    equipment.emplace(type, item);
}
