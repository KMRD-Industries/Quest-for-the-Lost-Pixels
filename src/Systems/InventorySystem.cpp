#include "InventorySystem.h"
#include "AnimationSystem.h"
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

void InventorySystem::dropItem(const Entity player, const Entity item, const GameType::slotType slot) const
{
    auto& equipmentComponent = gCoordinator.getComponent<EquipmentComponent>(player);
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

    auto weaponColliderComponent = ColliderComponent{true, "Weapon"};
    weaponColliderComponent.onCollisionEnter = [](const GameType::CollisionData& data) {};

    gCoordinator.removeComponent<ColliderComponent>(pickUpItemInfo.itemEntity);
    gCoordinator.addComponent(pickUpItemInfo.itemEntity, weaponColliderComponent);

    const Entity entity = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(
        pickUpItemInfo.itemEntity, "Weapon",
        [](const GameType::CollisionData& data)
        {
            auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(data.entityID);
            const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(data.entityID);
            auto& secondPlayertransformComponent = gCoordinator.getComponent<TransformComponent>(data.entityID);

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{});
            gCoordinator.addComponent(tag, TransformComponent{secondPlayertransformComponent});

            characterComponent.attacked = true;
            characterComponent.hp -= configSingleton.GetConfig().playerAttackDamage;

            const b2Vec2& attackerPos =
                gCoordinator.getComponent<ColliderComponent>(config::playerEntity).body->GetPosition();
            const b2Vec2& targetPos = colliderComponent.body->GetPosition();

            b2Vec2 recoilDirection = targetPos - attackerPos;
            recoilDirection.Normalize();

            const float& recoilMagnitude = 20.0f;
            const b2Vec2 recoilVelocity = recoilMagnitude * recoilDirection;

            secondPlayertransformComponent.velocity.x +=
                static_cast<float>(recoilVelocity.x * configSingleton.GetConfig().meterToPixelRatio);
            secondPlayertransformComponent.velocity.y +=
                static_cast<float>(recoilVelocity.y * configSingleton.GetConfig().meterToPixelRatio);
        },
        [](const GameType::CollisionData&) {}, false, false, true);

    gCoordinator.addComponent(entity, newEvent);


    if (gCoordinator.hasComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity))
        gCoordinator.removeComponent<ItemAnimationComponent>(pickUpItemInfo.itemEntity);

    gCoordinator.getComponent<ItemComponent>(pickUpItemInfo.itemEntity).equipped = true;

    auto& [equipment] = gCoordinator.getComponent<EquipmentComponent>(pickUpItemInfo.characterEntity);

    if (const auto it = equipment.find(pickUpItemInfo.slot); it != equipment.end())
        dropItem(pickUpItemInfo.characterEntity, it->second, it->first);

    equipment.emplace(pickUpItemInfo.slot, pickUpItemInfo.itemEntity);
}
