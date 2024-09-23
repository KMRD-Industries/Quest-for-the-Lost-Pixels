#include <format>

#include <comm.pb.h>

#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "CharacterSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "DoorComponent.h"
#include "DoorSystem.h"
#include "Dungeon.h"

#include <RenderSystem.h>

#include "CreateBodyWithCollisionEvent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "EquipWeaponSystem.h"
#include "EquippedWeaponComponent.h"
#include "FightSystem.h"
#include "FloorComponent.h"
#include "HealthBarSystem.h"
#include "InventoryComponent.h"
#include "InventorySystem.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PassageComponent.h"
#include "PassageSystem.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "SpawnerSystem.h"
#include "TextTagComponent.h"
#include "TextTagSystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "TravellingDungeonComponent.h"
#include "TravellingSystem.h"
#include "WeaponComponent.h"
#include "WeaponsSystem.h"

void Dungeon::init()
{
    m_entities = std::vector<Entity>(MAX_ENTITIES - 1);
    setECS();

    m_playerMovementSystem = gCoordinator.getRegisterSystem<PlayerMovementSystem>().get();
    m_multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>().get();
    m_characterSystem = gCoordinator.getRegisterSystem<CharacterSystem>().get();
    m_mapSystem = gCoordinator.getRegisterSystem<MapSystem>().get();
    m_doorSystem = gCoordinator.getRegisterSystem<DoorSystem>().get();
    m_passageSystem = gCoordinator.getRegisterSystem<PassageSystem>().get();
    m_travellingSystem = gCoordinator.getRegisterSystem<TravellingSystem>().get();
    m_textureSystem = gCoordinator.getRegisterSystem<TextureSystem>().get();
    m_animationSystem = gCoordinator.getRegisterSystem<AnimationSystem>().get();
    m_enemySystem = gCoordinator.getRegisterSystem<EnemySystem>().get();
    m_spawnerSystem = gCoordinator.getRegisterSystem<SpawnerSystem>().get();
    m_weaponSystem = gCoordinator.getRegisterSystem<WeaponSystem>().get();
    m_textTagSystem = gCoordinator.getRegisterSystem<TextTagSystem>().get();
    m_healthBarSystem = gCoordinator.getRegisterSystem<HealthBarSystem>().get();
    m_equipWeaponSystem = gCoordinator.getRegisterSystem<EquipWeaponSystem>().get();
    m_inventorySystem = gCoordinator.getRegisterSystem<InventorySystem>().get();

    auto _ = gCoordinator.createEntity(); // To evade Entity with ID == 0;

    const Entity player = gCoordinator.createEntity();
    config::playerEntity = player;
    m_id = config::playerEntity;

    m_multiplayerSystem->setup("127.0.0.1", "9001");

    if (m_multiplayerSystem->isConnected())
    {
        m_id = m_multiplayerSystem->registerPlayer(player);
        std::cout << "Connected to server with id: {}" << m_id;
    }
    else
        std::cout << "Starting in single-player mode";

    makeStartFloor();
    m_roomMap.at(m_currentPlayerPos).init();

    const auto mapPath = m_roomMap.at(m_currentPlayerPos).getMap();
    m_mapSystem->loadMap(mapPath);
    m_textureSystem->loadTextures();
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

    m_entities[m_id] = config::playerEntity;

    addPlayerComponents(m_entities[m_id]);
    setupPlayerCollision(m_entities[m_id]);
    setupWeaponEntity(m_entities[m_id]);
}

void Dungeon::addPlayerComponents(const Entity player)
{
    gCoordinator.addComponent(player, TileComponent{config::playerAnimation, "Characters", 6});
    gCoordinator.addComponent(player, RenderComponent{});
    gCoordinator.addComponent(player, TransformComponent{GameUtility::startingPosition});
    gCoordinator.addComponent(player, AnimationComponent{});
    gCoordinator.addComponent(player, CharacterComponent{.hp = config::defaultEnemyHP});
    gCoordinator.addComponent(player, PlayerComponent{});
    gCoordinator.addComponent(player, ColliderComponent{});
    gCoordinator.addComponent(player, InventoryComponent{});
    gCoordinator.addComponent(player, EquippedWeaponComponent{});
    gCoordinator.addComponent(player, FloorComponent{});
    gCoordinator.addComponent(
        player, TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) { moveInDungeon(dir); }});
    gCoordinator.addComponent(player, PassageComponent{.moveCallback = [this] { moveDownDungeon(); }});
}

void Dungeon::setupPlayerCollision(const Entity player)
{
    Collision cc = m_textureSystem->getCollision("Characters", config::playerAnimation);
    gCoordinator.getComponent<ColliderComponent>(player).collision = cc;

    auto onCollisionIn = [&](const GameType::CollisionData& entityT)
    {
        if (entityT.tag == "Door")
        {
            const auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entityT.entityID);
            auto& travellingDungeonComponent = gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[m_id]);

            if (travellingDungeonComponent.doorsPassed == 0)
            {
                travellingDungeonComponent.moveInDungeon.emplace_back(
                    GameType::mapDoorsToGeo.at(doorComponent.entrance));
            }
            ++travellingDungeonComponent.doorsPassed;
        }

        if (entityT.tag == "Passage")
        {
            auto& passageComponent = gCoordinator.getComponent<PassageComponent>(m_entities[m_id]);

            if (!passageComponent.activePassage) return;

            gCoordinator.getComponent<FloorComponent>(m_entities[m_id]).currentPlayerFloor += 1;
            passageComponent.moveInDungeon.emplace_back(true);
        }
    };

    auto onCollisionOut = [&](const GameType::CollisionData& entityT)
    {
        if (entityT.tag == "Door")
        {
            auto& travellingDungeonComponent = gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[m_id]);
            --travellingDungeonComponent.doorsPassed;
        }

        if (entityT.tag == "Passage")
        {
        }
    };

    const Entity entity = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(player, "Player 1", {cc.width, cc.height}, onCollisionIn,
                                                       onCollisionOut, false, false, {cc.x, cc.y});

    gCoordinator.addComponent(entity, newEvent);
}

void Dungeon::setupWeaponEntity(const Entity player)
{
    const Entity weaponEntity = gCoordinator.createEntity();

    gCoordinator.addComponent(weaponEntity, WeaponComponent{.id = 19});
    gCoordinator.addComponent(weaponEntity, TileComponent{19, "Weapons", 7});
    gCoordinator.addComponent(weaponEntity, TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(weaponEntity, RenderComponent{});
    gCoordinator.addComponent(weaponEntity, ColliderComponent{});
    gCoordinator.addComponent(weaponEntity, AnimationComponent{});

    gCoordinator.getRegisterSystem<InventorySystem>()->pickUpWeapon(player, weaponEntity);
    gCoordinator.getRegisterSystem<EquipWeaponSystem>()->equipWeapon(player, weaponEntity);
}

void Dungeon::draw()
{
    m_healthBarSystem->drawHealthBar();
    m_textureSystem->loadTextures();
    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::update()
{
    m_playerMovementSystem->update();
    m_spawnerSystem->update();
    m_enemySystem->update();
    m_travellingSystem->update();
    m_passageSystem->update();
    m_characterSystem->update();
    m_weaponSystem->update();
    m_animationSystem->update();
    m_textTagSystem->update();

    if (m_multiplayerSystem->isConnected())
    {
        constexpr int playerAnimationTile = 243;
        const auto stateUpdate = m_multiplayerSystem->pollStateUpdates();
        std::uint32_t id = stateUpdate.id();
        const auto tag = std::format("Player {}", id);

        switch (stateUpdate.variant())
        {
        case comm::DISCONNECTED:
            {
                gCoordinator.destroyEntity(m_entities[id]);
                m_multiplayerSystem->entityDisconnected(id);
                break;
            }
        case comm::CONNECTED:
            {
                m_entities[id] = gCoordinator.createEntity();

                gCoordinator.addComponent(m_entities[id], TileComponent{playerAnimationTile, "Characters", 6});
                gCoordinator.addComponent(m_entities[id], RenderComponent{});
                gCoordinator.addComponent(m_entities[id], TransformComponent{});
                gCoordinator.addComponent(m_entities[id], AnimationComponent{});
                gCoordinator.addComponent(m_entities[id], ColliderComponent{});
                gCoordinator.addComponent(m_entities[id], CharacterComponent{});
                gCoordinator.addComponent(m_entities[id], TextTagComponent{});

                // gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(m_entities[id], tag);

                const Entity entity = gCoordinator.createEntity();
                const auto newEvent = CreateBodyWithCollisionEvent(m_entities[id], tag, {}, {}, {}, false, false, {});
                gCoordinator.addComponent(entity, newEvent);

                m_multiplayerSystem->entityConnected(id, m_entities[id]);
                break;
            }
        default:
            break;
        }

        m_multiplayerSystem->update();
    }

    m_roomMap.at(m_currentPlayerPos).update();
}

void Dungeon::setECS()
{
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<AnimationComponent>();
    gCoordinator.registerComponent<DoorComponent>();
    gCoordinator.registerComponent<TravellingDungeonComponent>();
    gCoordinator.registerComponent<SpawnerComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<CharacterComponent>();
    gCoordinator.registerComponent<WeaponComponent>();
    gCoordinator.registerComponent<InventoryComponent>();
    gCoordinator.registerComponent<EquippedWeaponComponent>();
    gCoordinator.registerComponent<TextTagComponent>();
    gCoordinator.registerComponent<PassageComponent>();
    gCoordinator.registerComponent<FloorComponent>();

    auto playerMovementSystem = gCoordinator.getRegisterSystem<PlayerMovementSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<PlayerComponent>());
        gCoordinator.setSystemSignature<PlayerMovementSystem>(signature);
    }

    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<MultiplayerSystem>(signature);
    }

    auto characterSystem = gCoordinator.getRegisterSystem<CharacterSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<CharacterComponent>());
        gCoordinator.setSystemSignature<CharacterSystem>(signature);
    }

    auto mapSystem = gCoordinator.getRegisterSystem<MapSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<MapComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    auto doorSystem = gCoordinator.getRegisterSystem<DoorSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<DoorComponent>());
        gCoordinator.setSystemSignature<DoorSystem>(signature);
    }

    auto passageSystem = gCoordinator.getRegisterSystem<PassageSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<PassageComponent>());
        gCoordinator.setSystemSignature<PassageSystem>(signature);
    }

    auto travellingSystem = gCoordinator.getRegisterSystem<TravellingSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TravellingDungeonComponent>());
        gCoordinator.setSystemSignature<TravellingSystem>(signature);
    }

    const auto textureSystem = gCoordinator.getRegisterSystem<TextureSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<TextureSystem>(signature);
    }

    const auto animationSystem = gCoordinator.getRegisterSystem<AnimationSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<AnimationSystem>(signature);
    }

    const auto enemySystem = gCoordinator.getRegisterSystem<EnemySystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<EnemyComponent>());
        gCoordinator.setSystemSignature<EnemySystem>(signature);
    }

    const auto spawnerSystem = gCoordinator.getRegisterSystem<SpawnerSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<SpawnerComponent>());
        gCoordinator.setSystemSignature<SpawnerSystem>(signature);
    }

    const auto weaponSystem = gCoordinator.getRegisterSystem<WeaponSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<WeaponComponent>());
        gCoordinator.setSystemSignature<WeaponSystem>(signature);
    }

    const auto textTagSystem = gCoordinator.getRegisterSystem<TextTagSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TextTagComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<TextTagSystem>(signature);
    }

    const auto healthBarSystem = gCoordinator.getRegisterSystem<HealthBarSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<CharacterComponent>());
        signature.set(gCoordinator.getComponentType<PlayerComponent>());
        gCoordinator.setSystemSignature<HealthBarSystem>(signature);
    }

    const auto equipWeaponSystem = gCoordinator.getRegisterSystem<EquipWeaponSystem>();
    {
    }

    const auto inventorySystem = gCoordinator.getRegisterSystem<InventorySystem>();
    {
    }
}

void Dungeon::makeStartFloor()
{
    Room room{"0", 0};
    m_roomMap.clear();
    m_roomMap.emplace(glm::ivec2{0, 0}, room);
    m_currentPlayerPos = {0.f, 0.f};

    m_passageSystem->setPassages(true);
}

void Dungeon::makeSimpleFloor()
{
    const int playerFloor = gCoordinator.getComponent<FloorComponent>(config::playerEntity).currentPlayerFloor;
    const int floorID = config::m_mapDungeonLevelToFloorInfo.at(playerFloor);

    m_floorGenerator.setFloorID(floorID);
    m_textureSystem->modifyColorScheme(playerFloor);

    m_floorGenerator.generateFloor(5, 6);
    m_floorGenerator.generateMainPath(11);
    m_floorGenerator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.makeLockAndKey();

    m_roomMap = m_floorGenerator.getFloor(true);
    m_currentPlayerPos = m_floorGenerator.getStartingRoom();
    m_passageSystem->setPassages(m_floorGenerator.getEndingRoom() == m_currentPlayerPos);
}

void Dungeon::moveDownDungeon()
{
    makeSimpleFloor();
    const std::string& newMap = m_roomMap.at(m_currentPlayerPos).getMap();
    const auto& pos = GameUtility::startingPosition;

    m_doorSystem->clearDoors();
    m_spawnerSystem->clearSpawners();
    gCoordinator.getRegisterSystem<EnemySystem>()->deleteEnemies();
    m_passageSystem->setPassages(false);

    m_mapSystem->loadMap(newMap);
    m_textureSystem->loadTextures();
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

    gCoordinator.getComponent<TransformComponent>(m_entities[config::playerEntity]).position = {pos.x, pos.y};
    gCoordinator.getComponent<TransformComponent>(m_entities[config::playerEntity]).velocity = {};

    b2Vec2 position =
        gCoordinator.getComponent<ColliderComponent>(m_entities[config::playerEntity]).body->GetPosition();
    position.x = GameUtility::startingPosition.x * static_cast<float>(config::pixelToMeterRatio);
    position.y = GameUtility::startingPosition.y * static_cast<float>(config::pixelToMeterRatio);

    gCoordinator.getComponent<ColliderComponent>(m_entities[config::playerEntity]).body->SetTransform(position, 0);
}

void Dungeon::clearDungeon()
{
    m_doorSystem->clearDoors();
    m_spawnerSystem->clearSpawners();
    gCoordinator.getRegisterSystem<EnemySystem>()->deleteEnemies();
}

void Dungeon::moveInDungeon(const glm::ivec2& dir)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir))
    {
        clearDungeon();

        const std::uint32_t id = m_multiplayerSystem->playerID();
        m_currentPlayerPos += dir;
        const std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();

        m_doorSystem->clearDoors();
        m_spawnerSystem->clearSpawners();
        gCoordinator.getRegisterSystem<EnemySystem>()->deleteEnemies();
        m_mapSystem->loadMap(newMap);
        m_textureSystem->loadTextures();
        m_passageSystem->setPassages(m_currentPlayerPos == m_floorGenerator.getEndingRoom());
        gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = m_doorSystem->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};

        const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};
        gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;

        const auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);

        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
    }
}
