#include "ChestSpawnerSystem.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "HelmetComponent.h"
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
    m_helmetsIDs = gCoordinator.getRegisterSystem<TextureSystem>()->m_helmets;
    m_chestTile = TileComponent{819, "Items", 5};
}

void ChestSpawnerSystem::spawnChest() const
{
    for (const auto entity : m_entities)
    {
        processSpawn(gCoordinator.getComponent<TransformComponent>(entity));
    }
}

void ChestSpawnerSystem::spawnWeapon(const TransformComponent& spawnerTransformComponent) const
{
    const Entity newWeaponEntity = gCoordinator.createEntity();
    const auto& weaponDesc = getRandomElement(m_weaponsIDs);

    gCoordinator.addComponents(
        newWeaponEntity, WeaponComponent{.id = weaponDesc.first, .type = weaponDesc.second},
        TileComponent{static_cast<uint32_t>(weaponDesc.first), "Weapons", 7},
        TransformComponent{spawnerTransformComponent}, RenderComponent{}, ColliderComponent{}, AnimationComponent{},
        ItemComponent{},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y - 75});
}

void ChestSpawnerSystem::spawnHelmet(const TransformComponent& spawnerTransformComponent) const
{
    const Entity newHelmetEntity = gCoordinator.createEntity();
    const auto& helmetDesc = getRandomElement(m_helmetsIDs);

    gCoordinator.addComponents(
        newHelmetEntity, HelmetComponent{.id = helmetDesc},
        TileComponent{static_cast<uint32_t>(helmetDesc), "Armour", 7}, TransformComponent{spawnerTransformComponent},
        RenderComponent{}, ColliderComponent{}, AnimationComponent{}, ItemComponent{},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y - 75});
}

void ChestSpawnerSystem::clearSpawners() const {}

void ChestSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent) const
{
    // Create new entity for chest and add all necessary components
    const Entity newChestEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(newChestEntity, m_chestTile, spawnerTransformComponent,
                               ColliderComponent{m_chestCollision}, RenderComponent{}, AnimationComponent{},
                               CharacterComponent{}, ChestComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();

    auto spawnFunction = [this, spawnerTransformComponent](const GameType::CollisionData&)
    {
        // TODO: Spawning Logic
        spawnPotion(spawnerTransformComponent);
        spawnWeapon(spawnerTransformComponent);
        spawnHelmet(spawnerTransformComponent);
    };

    // Create new object with special eventComponent
    const auto newEventComponent = CreateBodyWithCollisionEvent(
        newChestEntity, "Chest", [this, newChestEntity](const GameType::CollisionData& collisionData)
        { handleChestCollision(newChestEntity, collisionData); }, spawnFunction, true, true);

    gCoordinator.addComponent(newItemEntity, newEventComponent);
}

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const GameType::CollisionData& collisionData) const
{
    if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void ChestSpawnerSystem::spawnPotion(const TransformComponent& spawnerTransformComponent) const
{
    const Entity newPotionEntity = gCoordinator.createEntity();
    const config::ItemConfig itemConfig = getRandomItemData();

    gCoordinator.addComponents(
        newPotionEntity, itemConfig.textureData, spawnerTransformComponent, ColliderComponent{m_potionCollision},
        RenderComponent{}, AnimationComponent{}, ChestComponent{}, CharacterComponent{},
        ItemComponent{itemConfig.name, itemConfig.value, itemConfig.behaviour, itemConfig.textureData},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y});
}