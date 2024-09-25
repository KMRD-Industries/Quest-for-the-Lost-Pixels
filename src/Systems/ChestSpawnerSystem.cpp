#include "ChestSpawnerSystem.h"

#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

ChestSpawnerSystem::ChestSpawnerSystem() { init(); }

void ChestSpawnerSystem::init()
{
    m_chestCollision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Items", 819);
    m_potionCollision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Items", 693);
    m_weaponsIDs = gCoordinator.getRegisterSystem<TextureSystem>()->m_weaponsIDs;
    m_chestTile = TileComponent{819, "Items", 5};
}

void ChestSpawnerSystem::spawnChest()
{
    for (const auto entity : m_entities)
    {
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        processSpawn(spawnerTransformComponent);
    }
}

void ChestSpawnerSystem::spawnWeapon(const TransformComponent& spawnerTransformComponent) const
{
    const Entity weapon = gCoordinator.createEntity();
    const auto& id = getRandomElement(m_weaponsIDs);

    gCoordinator.addComponents(
        weapon, WeaponComponent{.id = id}, TileComponent{static_cast<uint32_t>(id), "Weapons", 7},
        spawnerTransformComponent, RenderComponent{}, ColliderComponent{}, AnimationComponent{}, ItemComponent{},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y - 75});
}

void ChestSpawnerSystem::clearSpawners() const {}

void ChestSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent) const
{
    const Entity chest = gCoordinator.createEntity();

    gCoordinator.addComponents(chest, m_chestTile, spawnerTransformComponent, ColliderComponent{m_chestCollision},
                               RenderComponent{}, AnimationComponent{}, CharacterComponent{}, ChestComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();

    auto spawnFunction = [this, spawnerTransformComponent](const GameType::CollisionData&)
    { std::time(nullptr) % 3 == 0 ? spawnPotion(spawnerTransformComponent) : spawnWeapon(spawnerTransformComponent); };

    const auto newEvent = CreateBodyWithCollisionEvent(
        chest, "Chest", [this, chest](const GameType::CollisionData& collisionData)
        { handleChestCollision(chest, collisionData); }, spawnFunction, true, true);

    gCoordinator.addComponent(newItemEntity, newEvent);
}

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const GameType::CollisionData& collisionData) const
{
    if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void ChestSpawnerSystem::spawnPotion(const TransformComponent& spawnerTransformComponent) const
{
    const Entity potion = gCoordinator.createEntity();
    const config::ItemConfig itemConfig = getRandomItemData();

    gCoordinator.addComponents(
        potion, itemConfig.textureData, spawnerTransformComponent, ColliderComponent{m_potionCollision},
        RenderComponent{}, AnimationComponent{}, ChestComponent{}, CharacterComponent{},
        ItemComponent{itemConfig.name, itemConfig.value, itemConfig.behaviour, itemConfig.textureData},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y});
}