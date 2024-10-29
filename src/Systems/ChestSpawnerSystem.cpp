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
#include "MultiplayerSystem.h"
#include "PotionComponent.h"
#include "RenderComponent.h"
#include "SpawnerSystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "comm.pb.h"

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

void ChestSpawnerSystem::spawnItem(const TransformComponent &spawnerTransformComponent) const
{
    const Entity newItemEntity = gCoordinator.createEntity();

    gCoordinator.addComponents(newItemEntity, TransformComponent{spawnerTransformComponent}, RenderComponent{},
                               ColliderComponent{}, AnimationComponent{}, ItemComponent{});

    // for single player compatibility
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist1(1, 4);
    std::uniform_int_distribution<> dist2(0, 100);
    ItemGenerator itemGenerator{0, static_cast<uint32_t>(dist2(gen)), static_cast<comm::ItemType>(dist1(gen))};
    uint32_t id = 0;

    const auto& multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    if (multiplayerSystem->isConnected())
    {
        itemGenerator = multiplayerSystem->getItemGenerator();
        multiplayerSystem->registerItem(itemGenerator.id, newItemEntity);
    }

    switch (itemGenerator.type)
    {
        case comm::HELMET:
        {
            const uint32_t tileID = m_helmetsIDs[itemGenerator.gen % m_helmetsIDs.size()];
            if (id == 0) id = tileID;

            gCoordinator.addComponents(
                newItemEntity, HelmetComponent{.id = id},
                TileComponent{tileID, "Armour", 6},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
        case comm::WEAPON:
        {
            const auto& weaponDesc = m_weaponsIDs[itemGenerator.gen % m_weaponsIDs.size()];
            if (id == 0) id = weaponDesc.first;

            gCoordinator.addComponents(
                newItemEntity, WeaponComponent{.id = id, .type = weaponDesc.second},
                TileComponent{weaponDesc.first, "Weapons", 7},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y - 75});
            break;
        }
        case comm::ARMOUR:
        {
            const uint32_t tileID = m_bodyArmoursIDs[itemGenerator.gen % m_bodyArmoursIDs.size()];
            if (id == 0) id = tileID;

            gCoordinator.addComponents(
                newItemEntity, BodyArmourComponent{.id = id},
                TileComponent{tileID, "Armour", 6},
                ItemAnimationComponent{.animationDuration = 1,
                                       .startingPositionY = spawnerTransformComponent.position.y});
            break;
        }
        case comm::POTION:
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
        spawnItem(spawnerTransformComponent);
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
