#include "ChestSpawnerSystem.h"
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
    for (const auto entity : m_entities)
    {
        processSpawn(gCoordinator.getComponent<TransformComponent>(entity));
    }
}

void ChestSpawnerSystem::clearSpawners() const {}

void ChestSpawnerSystem::spawnItem(const TransformComponent &spawnerTransformComponent,
                                   const config::itemLootType itemType) const
{
    const Entity newItemEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(newItemEntity, TransformComponent{spawnerTransformComponent}, RenderComponent{},
                               ColliderComponent{}, AnimationComponent{}, ItemComponent{});

    switch (itemType)
    {
    case config::itemLootType::HELMET_LOOT:
        {
            const auto &helmetDesc = getRandomElement(m_helmetsIDs);
            gCoordinator.addComponents(
                newItemEntity, HelmetComponent{.id = helmetDesc},
                TileComponent{static_cast<uint32_t>(helmetDesc), "Armour", config::armourLayer},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
    case config::itemLootType::WEAPON_LOOT:
        {
            const auto weaponDesc = getRandomElement(m_weaponsIDs);
            gCoordinator.addComponents(
                newItemEntity, WeaponComponent{.id = weaponDesc.first, .type = weaponDesc.second},
                TileComponent{static_cast<uint32_t>(weaponDesc.first), "Weapons", config::weaponLayer},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y - 75});
            break;
        }
    case config::itemLootType::BODY_ARMOUR_LOOT:
        {
            const auto &bodyArmourDesc = getRandomElement(m_bodyArmoursIDs);
            gCoordinator.addComponents(
                newItemEntity, BodyArmourComponent{.id = bodyArmourDesc},
                TileComponent{static_cast<uint32_t>(bodyArmourDesc), "Armour", config::armourLayer},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
    case config::itemLootType::POTION_LOOT:
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

    gCoordinator.addComponents(newChestEntity, m_chestTile, TransformComponent{spawnerTransformComponent},
                               ColliderComponent{m_chestCollision}, RenderComponent{}, AnimationComponent{},
                               CharacterComponent{}, ItemComponent{}, ChestComponent{});

    const Entity newItemEntity = gCoordinator.createEntity();

    auto spawnFunction = [this, spawnerTransformComponent](const GameType::CollisionData &)
    {
        // TODO: Spawning Logic
        spawnItem(spawnerTransformComponent, config::itemLootType::POTION_LOOT);
        spawnItem(spawnerTransformComponent, config::itemLootType::WEAPON_LOOT);
        spawnItem(spawnerTransformComponent, config::itemLootType::HELMET_LOOT);
        spawnItem(spawnerTransformComponent, config::itemLootType::BODY_ARMOUR_LOOT);
    };

    // Create new object with special eventComponent
    const auto newEventComponent = CreateBodyWithCollisionEvent(
        newChestEntity, "Chest", [this, newChestEntity](const GameType::CollisionData &collisionData)
        { handleChestCollision(newChestEntity, collisionData); }, spawnFunction, true, true);

    gCoordinator.addComponent(newItemEntity, newEventComponent);
}

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const GameType::CollisionData &collisionData) const
{
    if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;
    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void ChestSpawnerSystem::spawnItem(const TransformComponent &spawnerTransformComponent) const
{
    const config::ItemConfig itemConfig = getRandomItemData();

    gCoordinator.addComponents(
        gCoordinator.createEntity(), itemConfig.textureData, TransformComponent{spawnerTransformComponent},
        ColliderComponent{m_potionCollision}, RenderComponent{}, AnimationComponent{}, ChestComponent{},
        PotionComponent{}, CharacterComponent{},
        ItemComponent{itemConfig.name, itemConfig.value, itemConfig.behaviour, itemConfig.textureData},
        ItemAnimationComponent{.animationDuration = 1, .startingPositionY = spawnerTransformComponent.position.y});
}
