#include "ChestSpawnerSystem.h"

#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "LootComponent.h"
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
}

void ChestSpawnerSystem::spawnChest()
{
    for (const auto entity : m_entities)
    {
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        processSpawn(spawnerTransformComponent);
    }
}

void ChestSpawnerSystem::spawnWeapon(const TransformComponent& spawnerTransformComponent)
{
    const Entity weapon = gCoordinator.createEntity();
    const auto& id = getRandomElement(m_weaponsIDs);

    // Spawn Weapon on the right side of spawner
    TransformComponent weaponSpawnPoint = spawnerTransformComponent;
    gCoordinator.addComponent(weapon, WeaponComponent{.id = id});
    gCoordinator.addComponent(weapon, TileComponent{static_cast<uint32_t>(id), "Weapons", 7});
    gCoordinator.addComponent(weapon, weaponSpawnPoint);
    gCoordinator.addComponent(weapon, RenderComponent{});
    gCoordinator.addComponent(weapon, ColliderComponent{});
    gCoordinator.addComponent(weapon, AnimationComponent{});
    gCoordinator.addComponent(weapon, ItemComponent{});
    gCoordinator.addComponent(
        weapon, ItemAnimationComponent{.animationDuration = 1, .startingPositionY = weaponSpawnPoint.position.y});
}

void ChestSpawnerSystem::clearSpawners() const {}

void ChestSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent)
{
    const Entity chest = gCoordinator.createEntity();
    const TileComponent tileComponent{819, "Items", 5};

    gCoordinator.addComponent(chest, tileComponent);
    gCoordinator.addComponent(chest, spawnerTransformComponent);
    gCoordinator.addComponent(chest, ColliderComponent{m_chestCollision});
    gCoordinator.addComponent(chest, RenderComponent{});
    gCoordinator.addComponent(chest, AnimationComponent{});
    gCoordinator.addComponent(chest, CharacterComponent{.hp = 100});
    gCoordinator.addComponent(chest, ChestComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();

    auto spawnFunction = [this, spawnerTransformComponent](const GameType::CollisionData& collisionData)
    { std::time(nullptr) % 3 == 0 ? spawnPotion(spawnerTransformComponent) : spawnWeapon(spawnerTransformComponent); };

    const auto newEvent = CreateBodyWithCollisionEvent(
        chest, "Chest", [this, chest, spawnerTransformComponent](const GameType::CollisionData& collisionData)
        { handleChestCollision(chest, spawnerTransformComponent, collisionData); }, spawnFunction, true, true);

    gCoordinator.addComponent(newItemEntity, newEvent);
}

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const TransformComponent& spawnerTransformComponent,
                                              const GameType::CollisionData& collisionData)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer) return;

    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void ChestSpawnerSystem::spawnPotion(const TransformComponent& spawnerTransformComponent)
{
    const Entity potion = gCoordinator.createEntity();
    const config::ItemConfig itemConfig = getRandomItemData();
    const TileComponent tileComponent{itemConfig.textureData};
    const ColliderComponent colliderComponent{m_potionCollision};

    gCoordinator.addComponent(potion, tileComponent);
    gCoordinator.addComponent(potion, spawnerTransformComponent);
    gCoordinator.addComponent(potion, colliderComponent);
    gCoordinator.addComponent(potion, RenderComponent{});
    gCoordinator.addComponent(potion, AnimationComponent{});
    gCoordinator.addComponent(potion, CharacterComponent{.hp = 100});
    gCoordinator.addComponent(
        potion, ItemComponent{itemConfig.name, itemConfig.value, itemConfig.behaviour, itemConfig.textureData});
    gCoordinator.addComponent(
        potion,
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y});
}