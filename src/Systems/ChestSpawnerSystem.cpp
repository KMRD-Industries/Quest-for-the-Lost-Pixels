#include "ChestSpawnerSystem.h"

#include <DirtyFlagComponent.h>

#include "AnimationComponent.h"
#include "BodyArmourComponent.h"
#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "HelmetComponent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "PotionComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

ChestSpawnerSystem::ChestSpawnerSystem() { init(); }

void ChestSpawnerSystem::init()
{
    m_chestCollision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Items", 819);
    m_potionCollision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Items", 693);
    m_weaponsIDs = gCoordinator.getRegisterSystem<TextureSystem>()->m_weaponsIDs;
    m_helmetsIDs = gCoordinator.getRegisterSystem<TextureSystem>()->m_helmets;
    m_bodyArmoursIDs = gCoordinator.getRegisterSystem<TextureSystem>()->m_bodyArmours;
    m_chestTile = TileComponent{819, "Items", 5};
}

void ChestSpawnerSystem::spawnChest() const
{
    for (const auto entity : m_entities) processSpawn(gCoordinator.getComponent<TransformComponent>(entity));
}

void ChestSpawnerSystem::clearSpawners() const {}

void ChestSpawnerSystem::spawnItem(const TransformComponent &spawnerTransformComponent,
                                   const GameType::itemLootType itemType) const
{
    const Entity newItemEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(newItemEntity, TransformComponent{spawnerTransformComponent});
    gCoordinator.addComponents(newItemEntity, RenderComponent{});
    gCoordinator.addComponents(newItemEntity, ColliderComponent{});
    gCoordinator.addComponents(newItemEntity, AnimationComponent{});
    gCoordinator.addComponents(newItemEntity, ItemComponent{});
    gCoordinator.addComponents(newItemEntity, DirtyFlagComponent{});

    switch (itemType)
    {
    case GameType::itemLootType::HELMET_LOOT:
        {
            const auto &helmetDesc = getRandomElement(m_helmetsIDs);
            gCoordinator.addComponents(
                newItemEntity, HelmetComponent{.id = helmetDesc},
                TileComponent{static_cast<uint32_t>(helmetDesc), "Armour", 6},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
    case GameType::itemLootType::WEAPON_LOOT:
        {
            const auto weaponDesc = getRandomElement(m_weaponsIDs);
            gCoordinator.addComponents(
                newItemEntity, WeaponComponent{.id = weaponDesc.first, .type = weaponDesc.second},
                TileComponent{static_cast<uint32_t>(weaponDesc.first), "Weapons", 7},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y - 75});
            break;
        }
    case GameType::itemLootType::BODY_ARMOUR_LOOT:
        {
            const auto &bodyArmourDesc = getRandomElement(m_bodyArmoursIDs);
            gCoordinator.addComponents(
                newItemEntity, BodyArmourComponent{.id = bodyArmourDesc},
                TileComponent{static_cast<uint32_t>(bodyArmourDesc), "Armour", 6},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
    case GameType::itemLootType::POTION_LOOT:
        {
            const config::ItemConfig itemConfig = getRandomItemData();
            gCoordinator.addComponents(
                newItemEntity, PotionComponent{}, ChestComponent{}, CharacterComponent{}, itemConfig.textureData,
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
    default:;
    }
}

void ChestSpawnerSystem::processSpawn(const TransformComponent &spawnerTransformComponent) const
{
    // Create new entity for chest and add all necessary components
    const Entity newChestEntity = gCoordinator.createEntity();

    gCoordinator.addComponent(newChestEntity, m_chestTile);
    gCoordinator.addComponent(newChestEntity, TransformComponent{.position = spawnerTransformComponent.position});
    gCoordinator.addComponent(newChestEntity, ColliderComponent{.collision = m_chestCollision});
    gCoordinator.addComponent(newChestEntity, RenderComponent{});
    gCoordinator.addComponent(newChestEntity, AnimationComponent{});
    gCoordinator.addComponent(newChestEntity, CharacterComponent{});
    gCoordinator.addComponent(newChestEntity, ItemComponent{});
    gCoordinator.addComponent(newChestEntity, ChestComponent{});
    gCoordinator.addComponent(newChestEntity, DirtyFlagComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();

    auto spawnFunction = [this, spawnerTransformComponent](const GameType::CollisionData &)
    {
        // TODO: Spawn logic
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1, 4);

        switch (dist(gen))
        {
        case 1:
            spawnItem(spawnerTransformComponent, GameType::itemLootType::POTION_LOOT);
            break;
        case 2:
            spawnItem(spawnerTransformComponent, GameType::itemLootType::WEAPON_LOOT);
            break;
        case 3:
            spawnItem(spawnerTransformComponent, GameType::itemLootType::HELMET_LOOT);
            break;
        case 4:
            spawnItem(spawnerTransformComponent, GameType::itemLootType::BODY_ARMOUR_LOOT);
            break;
        default:
            break;
        }
    };

    // Create new object with special eventComponent
    const auto newEventComponent = CreateBodyWithCollisionEvent{
        .entity = newChestEntity,
        .tag = "Chest",
        .onCollisionEnter = [this, newChestEntity](const GameType::CollisionData &collisionData)
        { handleChestCollision(newChestEntity, collisionData); },
        .onCollisionOut = spawnFunction,
        .isStatic = true,
        .useTextureSize = true};

    gCoordinator.addComponent(newItemEntity, newEventComponent);
}

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const GameType::CollisionData &collisionData) const
{
    if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}