#include "InventorySystem.h"
#include <DirtyFlagComponent.h>
#include "AnimationSystem.h"
#include "BindSwingWeaponEvent.h"
#include "BodyArmourComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquipmentComponent.h"
#include "GameTypes.h"
#include "HelmetComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "PotionComponent.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

void InventorySystem::dropItem(const Entity player, const Entity item, const GameType::slotType slot) const
{
    auto &equipmentComponent = gCoordinator.getComponent<EquipmentComponent>(player);
    const auto &playerTransformComponent = gCoordinator.getComponent<TransformComponent>(item);

    const Entity newItemEntity = gCoordinator.createEntity();

    const auto itemAnimationComponent =
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = playerTransformComponent.position.y - 75};

    gCoordinator.addComponents(newItemEntity, TileComponent{gCoordinator.getComponent<TileComponent>(item)});
    gCoordinator.addComponents(newItemEntity, RenderComponent{});
    gCoordinator.addComponents(newItemEntity, ColliderComponent{});
    gCoordinator.addComponents(newItemEntity, AnimationComponent{});
    gCoordinator.addComponents(newItemEntity, ItemComponent{});
    gCoordinator.addComponents(newItemEntity, DirtyFlagComponent{});
    gCoordinator.addComponents(newItemEntity, itemAnimationComponent);
    gCoordinator.addComponents(newItemEntity, TransformComponent{playerTransformComponent});

    switch (slot)
    {
    case GameType::slotType::WEAPON:
        {
            gCoordinator.addComponent(newItemEntity, gCoordinator.getComponent<WeaponComponent>(item));
            break;
        }
    case GameType::slotType::HELMET:
        {
            gCoordinator.addComponent(newItemEntity, gCoordinator.getComponent<HelmetComponent>(item));
            break;
        }
    case GameType::slotType::BODY_ARMOUR:
        {
            gCoordinator.addComponent(newItemEntity, gCoordinator.getComponent<BodyArmourComponent>(item));
            break;
        }
    }

    gCoordinator.destroyEntity(equipmentComponent.slots.at(slot));
    equipmentComponent.slots.erase(slot);
}

void InventorySystem::pickUpItem(const GameType::PickUpInfo pickUpItemInfo) const
{
    if (gCoordinator.hasComponent<PotionComponent>(pickUpItemInfo.itemEntity)) return;
    gCoordinator.getRegisterSystem<CollisionSystem>()->deleteBody(pickUpItemInfo.itemEntity);

    if (gCoordinator.hasComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity))
        gCoordinator.removeComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity);

    gCoordinator.getComponent<ItemComponent>(pickUpItemInfo.itemEntity).equipped = true;

    auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(pickUpItemInfo.characterEntity);

    if (const auto it = equipment.find(pickUpItemInfo.slot); it != equipment.end())
        dropItem(pickUpItemInfo.characterEntity, it->second, it->first);

    equipment.emplace(pickUpItemInfo.slot, pickUpItemInfo.itemEntity);

    if (pickUpItemInfo.slot == GameType::WEAPON)
    {
        gCoordinator.addComponent(pickUpItemInfo.itemEntity, BindSwingWeaponEvent{});
        gCoordinator.addComponent(pickUpItemInfo.itemEntity, WeaponSwingComponent{});
    }
}