#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipmentComponent.h"
#include "HelmetComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "PotionComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

void InventorySystem::dropItem(const Entity player, const Entity item, const GameType::slotType slot) const
{
    auto &[equipment] = gCoordinator.getComponent<EquipmentComponent>(player);
    const Entity newItemEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(
        newItemEntity, TileComponent{gCoordinator.getComponent<TileComponent>(item)},
        TransformComponent{gCoordinator.getComponent<TransformComponent>(player)}, RenderComponent{},
        ColliderComponent{}, AnimationComponent{}, ItemComponent{},
        ItemAnimationComponent{
            .animationDuration = 1,
            .startingPositionY = gCoordinator.getComponent<TransformComponent>(player).position.y - 75});

    switch (slot)
    {
    case GameType::slotType::WEAPON:
        {
            gCoordinator.addComponent(newItemEntity, WeaponComponent{gCoordinator.getComponent<WeaponComponent>(item)});
            break;
        }
    case GameType::slotType::HELMET:
        {
            gCoordinator.addComponent(newItemEntity, HelmetComponent{gCoordinator.getComponent<HelmetComponent>(item)});
            break;
        }
    case GameType::slotType::BODY_ARMOUR:
        {
            gCoordinator.addComponent(newItemEntity,
                                      BodyArmourComponent{gCoordinator.getComponent<BodyArmourComponent>(item)});
            break;
        }
    default:
        break;
    }

    gCoordinator.destroyEntity(equipment.at(slot));
    equipment.erase(slot);
}

void InventorySystem::pickUpItem(const GameType::PickUpInfo pickUpItemInfo) const
{
    if (gCoordinator.hasComponent<PotionComponent>(pickUpItemInfo.itemEntity)) return;

    if (const auto *colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(pickUpItemInfo.itemEntity))
        if (colliderComponent->body != nullptr) Physics::getWorld()->DestroyBody(colliderComponent->body);

    if (gCoordinator.hasComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity))
        gCoordinator.removeComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity);

    gCoordinator.getComponent<ItemComponent>(pickUpItemInfo.itemEntity).equipped = true;

    auto &[equipment] = gCoordinator.getComponent<EquipmentComponent>(pickUpItemInfo.characterEntity);

    if (const auto it = equipment.find(pickUpItemInfo.slot); it != equipment.end())
        dropItem(pickUpItemInfo.characterEntity, it->second, it->first);

    equipment.emplace(pickUpItemInfo.slot, pickUpItemInfo.itemEntity);
}
