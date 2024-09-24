#include <comm.pb.h>
#include <format>

#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "CharacterSystem.h"
#include "ChestComponent.h"
#include "ChestSpawnerSystem.h"
#include "ChestSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "DoorComponent.h"
#include "DoorSystem.h"
#include "Dungeon.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "EquipWeaponSystem.h"
#include "EquippedWeaponComponent.h"
#include "FloorComponent.h"
#include "HealthBarSystem.h"
#include "InventoryComponent.h"
#include "InventorySystem.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "ItemSpawnerSystem.h"
#include "LootComponent.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "MultiplayerComponent.h"
#include "MultiplayerSystem.h"
#include "PassageComponent.h"
#include "PassageSystem.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "RoomListenerSystem.h"
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

extern Coordinator Coordinator;

void Dungeon::init()
{
    setECS();
    auto _ = gCoordinator.createEntity(); // Ignore Entity with ID = 0
    config::playerEntity = gCoordinator.createEntity();
    m_id = 0;
    m_seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_multiplayerSystem->setup("127.0.0.1", "9001");

    if (m_multiplayerSystem->isConnected())
    {
        const auto gameState = m_multiplayerSystem->registerPlayer(config::playerEntity);
        m_id = gameState.player_id();
        m_seed = gameState.seed();

        std::cout << "Connected to server with id: " << m_id << "\n";

        for (uint32_t id : gameState.connected_players())
        {
            createRemotePlayer(id);
            std::cout << "Connected remote player: " << m_id << "\n";
        }
    }
    else
        std::cout << "Starting in single-player mode";

    const std::string tag = std::format("Player {}", m_id);

    makeStartFloor();
    m_roomMap.at(m_currentPlayerPos).init();
    loadMap(m_roomMap.at(m_currentPlayerPos).getMap());

    m_entities[m_id] = config::playerEntity;
    m_players.insert(m_id);

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
    gCoordinator.addComponent(player, CharacterComponent{.hp = config::defaultCharacterHP});
    gCoordinator.addComponent(player, PlayerComponent{});
    gCoordinator.addComponent(player, ColliderComponent{});
    gCoordinator.addComponent(player, InventoryComponent{});
    gCoordinator.addComponent(player, EquippedWeaponComponent{});
    gCoordinator.addComponent(player, FloorComponent{});
    gCoordinator.addComponent(
        player, TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) { moveInDungeon(dir); }});
    gCoordinator.addComponent(player, PassageComponent{.moveCallback = [this] { moveDownDungeon(); }});
}

void Dungeon::createRemotePlayer(const uint32_t id)
{
    const auto tag = std::format("Player {}", id);
    m_entities[id] = gCoordinator.createEntity();

    gCoordinator.addComponent(m_entities[id],
                              TransformComponent(sf::Vector2f(getSpawnOffset(config::startingPosition.x, id),
                                                              getSpawnOffset(config::startingPosition.y, id)),
                                                 0.f, sf::Vector2f(1.f, 1.f), {0.f, 0.f}));
    gCoordinator.addComponent(m_entities[id], TileComponent{config::playerAnimation, "Characters", 3});
    gCoordinator.addComponent(m_entities[id], RenderComponent{});
    gCoordinator.addComponent(m_entities[id], AnimationComponent{});
    gCoordinator.addComponent(m_entities[id], CharacterComponent{.hp = config::defaultCharacterHP});
    gCoordinator.addComponent(m_entities[id], MultiplayerComponent{});
    gCoordinator.addComponent(m_entities[id], ColliderComponent{});

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", config::playerAnimation);
    gCoordinator.getComponent<ColliderComponent>(m_entities[id]).collision = cc;

    const Entity entity = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(
        m_entities[id], tag, [&](const GameType::CollisionData&) {}, [&](const GameType::CollisionData&) {}, false,
        false);

    gCoordinator.addComponent(entity, newEvent);


    m_multiplayerSystem->entityConnected(id, m_entities[id]);
}

void Dungeon::setupPlayerCollision(const Entity player)
{
    const auto& cc = m_textureSystem->getCollision("Characters", config::playerAnimation);
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
            passageComponent.activePassage = false;
        }
    };

    auto onCollisionOut = [&](const GameType::CollisionData& entityT)
    {
        if (entityT.tag == "Door")
        {
            auto& travellingDungeonComponent = gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[m_id]);
            --travellingDungeonComponent.doorsPassed;
        }
    };

    const Entity entity = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(player, "Player 1", onCollisionIn, onCollisionOut, false, false);

    gCoordinator.addComponent(entity, newEvent);
}

void Dungeon::setupWeaponEntity(const Entity player) const
{
    const Entity weaponEntity = gCoordinator.createEntity();

    gCoordinator.addComponent(weaponEntity, WeaponComponent{.id = 19});
    gCoordinator.addComponent(weaponEntity, TileComponent{19, "Weapons", 7});
    gCoordinator.addComponent(weaponEntity, TransformComponent{});
    gCoordinator.addComponent(weaponEntity, RenderComponent{});
    gCoordinator.addComponent(weaponEntity, ColliderComponent{});
    gCoordinator.addComponent(weaponEntity, AnimationComponent{});

    m_inventorySystem->pickUpWeapon(player, weaponEntity);
    m_equipWeaponSystem->equipWeapon(player, weaponEntity);
}

void Dungeon::draw()
{
    m_healthBarSystem->drawHealthBar();
    m_textureSystem->loadTextures();
    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::update(const float deltaTime)
{
    m_playerMovementSystem->update();
    m_weaponSystem->update();
    m_spawnerSystem->update();
    m_enemySystem->update();
    m_travellingSystem->update();
    m_passageSystem->update();
    m_characterSystem->update();
    m_animationSystem->update();
    m_textTagSystem->update();
    m_roomListenerSystem->update();
    m_itemSpawnerSystem->updateAnimation(deltaTime);

    if (m_multiplayerSystem->isConnected())
    {
        const auto stateUpdate = m_multiplayerSystem->pollStateUpdates();
        const uint32_t id = stateUpdate.id();

        switch (stateUpdate.variant())
        {
        case comm::DISCONNECTED:
            gCoordinator.destroyEntity(m_entities[id]);
            m_multiplayerSystem->entityDisconnected(id);
            m_players.erase(id);
            break;
        case comm::CONNECTED:
            createRemotePlayer(id);
            m_players.insert(id);
            break;
        case comm::ROOM_CHANGED:
            changeRoom(m_multiplayerSystem->getRoom());
            break;
        default:
            break;
        }

        m_multiplayerSystem->update();
    }

    m_roomMap.at(m_currentPlayerPos).update();
}

void Dungeon::changeRoom(const glm::ivec2& room)
{
    clearDungeon();
    auto dir = room - m_currentPlayerPos;
    m_currentPlayerPos = room;

    loadMap(m_roomMap.at(m_currentPlayerPos).getMap());

    const auto newDoor = dir * -1;
    const auto doorType = GameType::geoToMapDoors.at(newDoor);
    const auto position = m_doorSystem->getDoorPosition(doorType);
    const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};

    for (const auto id : m_players)
    {
        const sf::Vector2f newPosition = position + offset + GameType::MyVec2(id, id);
        gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);

        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
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

    m_floorGenerator.generateFloor(5, 6, m_seed);
    m_floorGenerator.generateMainPath(11);
    m_floorGenerator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.makeLockAndKey();

    m_roomMap = m_floorGenerator.getFloor(true);
    m_currentPlayerPos = m_floorGenerator.getStartingRoom();
    m_passageSystem->setPassages(m_floorGenerator.getEndingRoom() == m_currentPlayerPos);
    m_roomListenerSystem->changeRoom(m_currentPlayerPos);
}

void Dungeon::moveDownDungeon()
{
    makeSimpleFloor();
    const auto& pos = GameUtility::startingPosition;

    clearDungeon();
    m_passageSystem->setPassages(false);
    loadMap(m_roomMap.at(m_currentPlayerPos).getMap());

    gCoordinator.getComponent<TransformComponent>(config::playerEntity).position = {pos.x, pos.y};
    gCoordinator.getComponent<TransformComponent>(config::playerEntity).velocity = {};

    b2Vec2 position{};
    position.x = GameUtility::startingPosition.x * static_cast<float>(config::pixelToMeterRatio);
    position.y = GameUtility::startingPosition.y * static_cast<float>(config::pixelToMeterRatio);

    gCoordinator.getComponent<ColliderComponent>(config::playerEntity).body->SetTransform(position, 0);
}

inline void Dungeon::loadMap(const std::string& path) const
{
    m_mapSystem->loadMap(path);
    m_textureSystem->loadTextures();
    m_collisionSystem->createMapCollision();
}

inline void Dungeon::clearDungeon() const
{
    m_doorSystem->clearDoors();
    m_spawnerSystem->clearSpawners();
    m_enemySystem->deleteEnemies();
    m_itemSpawnerSystem->deleteItems();
    m_chestSystem->deleteItems();
}

void Dungeon::moveInDungeon(const glm::ivec2& dir)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir))
    {
        clearDungeon();
        m_currentPlayerPos += dir;
        m_roomListenerSystem->changeRoom(m_currentPlayerPos);

        m_mapSystem->loadMap(m_roomMap.at(m_currentPlayerPos).getMap());
        m_textureSystem->loadTextures();
        m_passageSystem->setPassages(m_currentPlayerPos == m_floorGenerator.getEndingRoom());
        m_collisionSystem->createMapCollision();

        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};

        for (const auto id : m_players)
        {
            // make players spawn in different positions after room change
            // there should be a better way to do this
            const sf::Vector2f newPosition = {position.x + offset.x + id, position.y + offset.y + id};

            gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;
            const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);
            colliderComponent.body->SetTransform(
                {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
                colliderComponent.body->GetAngle());
        }

        if (m_multiplayerSystem->isConnected()) m_multiplayerSystem->roomChanged(m_currentPlayerPos);
    }
}

float Dungeon::getSpawnOffset(const float position, const int id)
{
    if (id % 2 == 0) return position + id * config::spawnOffset;
    return position - id * config::spawnOffset;
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
    gCoordinator.registerComponent<MultiplayerComponent>();
    gCoordinator.registerComponent<LootComponent>();
    gCoordinator.registerComponent<ItemComponent>();
    gCoordinator.registerComponent<ItemAnimationComponent>();
    gCoordinator.registerComponent<ChestComponent>();

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
        signature.set(gCoordinator.getComponentType<MultiplayerComponent>());
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
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

    const auto chestSpawnerSystem = gCoordinator.getRegisterSystem<ChestSpawnerSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<LootComponent>());
        gCoordinator.setSystemSignature<ChestSpawnerSystem>(signature);
    }

    const auto lootSpawnerSystem = gCoordinator.getRegisterSystem<ItemSpawnerSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<ItemAnimationComponent>());
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<ItemSpawnerSystem>(signature);
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

    const auto roomListenerSystem = gCoordinator.getRegisterSystem<RoomListenerSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<EnemyComponent>());
        gCoordinator.setSystemSignature<RoomListenerSystem>(signature);
    }

    const auto chestSystem = gCoordinator.getRegisterSystem<ChestSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<CharacterComponent>());
        signature.set(gCoordinator.getComponentType<ChestComponent>());
        gCoordinator.setSystemSignature<ChestSystem>(signature);
    }

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
    m_collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>().get();
    m_chestSystem = gCoordinator.getRegisterSystem<ChestSystem>().get();
    m_roomListenerSystem = gCoordinator.getRegisterSystem<RoomListenerSystem>().get();
    m_itemSpawnerSystem = gCoordinator.getRegisterSystem<ItemSpawnerSystem>().get();
}
