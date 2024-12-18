#include "InventorySystem.h"
#include "AnimationSystem.h"
#include "BindSwingWeaponEvent.h"
#include "BodyArmourComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EquipmentComponent.h"
#include "GameTypes.h"
#include "HelmetComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "PotionComponent.h"
#include "RenderComponent.h"
#include "SpawnerSystem.h"
#include "SynchronisedEvent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSwingComponent.h"

void InventorySystem::dropItem(const Entity player, const Entity item, const GameType::slotType slot) const
{
    auto& equipmentComponent = gCoordinator.getComponent<EquipmentComponent>(player);
    const Entity newItemEntity = gCoordinator.createEntity();

    const Entity eventEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(
        eventEntity,
        SynchronisedEvent{
            .variant = SynchronisedEvent::Variant::UPDATE_ITEM_ENTITY, .entity = item, .updatedEntity = newItemEntity});

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
        if (!gCoordinator.hasComponent<BindSwingWeaponEvent>(pickUpItemInfo.itemEntity))
            gCoordinator.addComponent(pickUpItemInfo.itemEntity, BindSwingWeaponEvent{});

        if (!gCoordinator.hasComponent<WeaponSwingComponent>(pickUpItemInfo.itemEntity))
            gCoordinator.addComponent(pickUpItemInfo.itemEntity, WeaponSwingComponent{});
    }
}
