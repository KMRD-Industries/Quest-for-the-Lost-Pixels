#include <chrono>
#include <format>
#include <iostream>
#include <vector>

#include <comm.pb.h>

#include "Dungeon.h"
#include <BodyArmourComponent.h>
#include <CreateBodyWithCollisionEvent.h>
#include <EquipmentComponent.h>
#include <FloorComponent.h>
#include <HelmetComponent.h>
#include <ItemSystem.h>
#include <PassageComponent.h>
#include <PotionComponent.h>
#include <RenderSystem.h>
#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include "BindSwingWeaponEvent.h"
#include "CharacterComponent.h"
#include "CharacterSystem.h"
#include "ChestComponent.h"
#include "ChestSpawnerSystem.h"
#include "ChestSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Config.h"
#include "DealDMGToEnemyEvent.h"
#include "DoorComponent.h"
#include "DoorSystem.h"

#include <map>

#include "EndGameState.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "HealthBarSystem.h"
#include "InputHandler.h"
#include "InventoryComponent.h"
#include "InventorySystem.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "ItemSpawnerSystem.h"
#include "LootComponent.h"
#include "LostGameState.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "SynchronisedEvent.h"
#include "MultiplayerSystem.h"
#include "ObjectCreatorSystem.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
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
#include "WeaponSwingComponent.h"
#include "WeaponsSystem.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

void Dungeon::init()
{
    setECS();

    m_timer = Timer::Instance();
    auto _ = gCoordinator.createEntity(); // Ignore Entity with ID = 0
    config::playerEntity = gCoordinator.createEntity();

    comm::Player player;

    m_id = 0;
    m_seed = std::chrono::system_clock::now().time_since_epoch().count();

    const std::string IP = ResourceManager::getInstance().getIP();
    size_t colonPos = IP.find(':');
    if (colonPos == std::string::npos)
        m_multiplayerSystem->setup(IP, config::defaultPort);
    else
    {
        std::string ip = IP.substr(0, colonPos);
        std::string port = IP.substr(colonPos + 1);
        m_multiplayerSystem->setup(ip, port);
    }

    if (m_multiplayerSystem->isConnected())
    {
        const auto& initialInfo = m_multiplayerSystem->registerPlayer(config::playerEntity);
        player = initialInfo.player();
        m_id = player.id();
        m_seed = initialInfo.seed();

        std::cout << "Connected to server with id: " << m_id << '\n';

        for (const auto& player: initialInfo.connected_players())
        {
            const uint32_t playerID = player.id();
            createRemotePlayer(player);
            std::cout << "Connected remote player: " << playerID << '\n';
        }
    }
    else
    {
        m_multiplayerSystem->setPlayer(m_id, config::playerEntity);
        std::cout << "Starting in single-player mode\n";
    }

    if (m_multiplayerSystem->isConnected()) m_multiplayerSystem->setRoom(m_currentPlayerPos);

    makeStartFloor();
    m_roomMap.at(m_currentPlayerPos).init();
    loadMap(m_roomMap.at(m_currentPlayerPos).getMap());

    m_entities[m_id] = config::playerEntity;

    addPlayerComponents(m_entities[m_id]);
    setupPlayerCollision(m_entities[m_id]);
    setupWeaponEntity(player);
    setupHelmetEntity(player);
    m_passageSystem->setPassages(true);
}

void Dungeon::render(sf::RenderTexture& window)
{
    gCoordinator.getRegisterSystem<TextureSystem>().get()->loadTextures();
    gCoordinator.getRegisterSystem<HealthBarSystem>().get()->drawHealthBar();
    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::addPlayerComponents(const Entity player)
{
    gCoordinator.addComponents(
        player, TileComponent{configSingleton.GetConfig().playerAnimation, "Characters", 5}, RenderComponent{},
        TransformComponent{GameUtility::startingPosition}, AnimationComponent{},
        CharacterComponent{.hp = configSingleton.GetConfig().defaultCharacterHP}, PlayerComponent{},
        ColliderComponent{}, InventoryComponent{}, EquipmentComponent{}, FloorComponent{},
        TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) { moveInDungeon(dir, true); }},
        PassageComponent{.moveCallback = [this] { moveDownDungeon(true); }});
}

void Dungeon::createRemotePlayer(const comm::Player& player)
{
    const uint32_t playerID = player.id();
    const auto tag = std::format("Player {}", playerID);
    m_entities[playerID] = gCoordinator.createEntity();

    gCoordinator.addComponents(m_entities[playerID],
        TransformComponent(sf::Vector2f(getSpawnOffset(configSingleton.GetConfig().startingPosition.x, playerID),
                                        getSpawnOffset(configSingleton.GetConfig().startingPosition.y, playerID)),
                           0.f, sf::Vector2f(1.f, 1.f), {0.f, 0.f}),
        TileComponent{configSingleton.GetConfig().playerAnimation, "Characters", 5},
        RenderComponent{},
        AnimationComponent{},
        CharacterComponent{.hp = configSingleton.GetConfig().defaultCharacterHP},
        PlayerComponent{},
        ColliderComponent{},
        InventoryComponent{},
        EquipmentComponent{}
    );

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", configSingleton.GetConfig().playerAnimation);
    gCoordinator.getComponent<ColliderComponent>(m_entities[playerID]).collision = cc;

    const Entity entity = gCoordinator.createEntity();
    const auto newEvent = CreateBodyWithCollisionEvent(
        m_entities[playerID], tag, [&](const GameType::CollisionData&) {}, [&](const GameType::CollisionData&) {}, false,
        false);

    gCoordinator.addComponent(entity, newEvent);

    setupWeaponEntity(player);
    setupHelmetEntity(player);

    const Entity eventEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(eventEntity,
                              SynchronisedEvent{.variant = SynchronisedEvent::Variant::PLAYER_CONNECTED,
                                                .entityID = playerID,
                                                .entity = m_entities[playerID]});
}

void Dungeon::setupPlayerCollision(const Entity player)
{
    const auto& cc = m_textureSystem->getCollision("Characters", configSingleton.GetConfig().playerAnimation);
    gCoordinator.getComponent<ColliderComponent>(player).collision = cc;
    const std::string tag = std::format("Player {}", m_id);

    auto onCollisionIn = [&](const GameType::CollisionData& entityT)
    {
        if (entityT.tag == "Door")
        {
            const auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entityT.entity);
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
    const auto newEvent = CreateBodyWithCollisionEvent(player, tag, onCollisionIn, onCollisionOut, false, false);

    gCoordinator.addComponent(entity, newEvent);
}

void Dungeon::setupWeaponEntity(const comm::Player& player) const
{
    // temporary for single-player
    comm::Item weapon;
    if (m_multiplayerSystem->isConnected())
        weapon = player.items(0);

    const auto& availableWeapons = gCoordinator.getRegisterSystem<TextureSystem>()->m_weaponsIDs;

    const auto& weaponDesc = availableWeapons[weapon.gen() % availableWeapons.size()];

    const Entity weaponEntity = gCoordinator.createEntity();
    const Entity playerEntity = m_entities[player.id()];

    gCoordinator.addComponent(weaponEntity, WeaponComponent{.id = weaponDesc.first, .type = weaponDesc.second});
    gCoordinator.addComponent(weaponEntity, TileComponent{weaponDesc.first, "Weapons", 7});
    gCoordinator.addComponent(weaponEntity, TransformComponent{});
    gCoordinator.addComponent(weaponEntity, RenderComponent{});
    gCoordinator.addComponent(weaponEntity, AnimationComponent{});
    gCoordinator.addComponent(weaponEntity, ItemAnimationComponent{});
    gCoordinator.addComponent(weaponEntity, CharacterComponent{});
    gCoordinator.addComponent(weaponEntity, ItemComponent{.equipped = true});

    m_inventorySystem->pickUpItem(GameType::PickUpInfo{playerEntity, weaponEntity, GameType::slotType::WEAPON});

    const Entity eventEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(
        eventEntity,
        SynchronisedEvent{
            .variant = SynchronisedEvent::Variant::REGISTER_ITEM, .entityID = weapon.id(), .entity = weaponEntity});
}

void Dungeon::setupHelmetEntity(const comm::Player& player) const
{
    // temporary for single-player
    comm::Item helmet;
    if (m_multiplayerSystem->isConnected())
        helmet = player.items(1);

    const auto& availableHelmets = gCoordinator.getRegisterSystem<TextureSystem>()->m_helmets;

    const uint32_t helmetID = availableHelmets[helmet.gen() % availableHelmets.size()];

    const Entity helmetEntity = gCoordinator.createEntity();
    const Entity playerEntity = m_entities[player.id()];

    gCoordinator.addComponent(helmetEntity, HelmetComponent{.id = helmetID});
    gCoordinator.addComponent(helmetEntity, TileComponent{helmetID, "Armour", 6});
    gCoordinator.addComponent(helmetEntity, TransformComponent{});
    gCoordinator.addComponent(helmetEntity, RenderComponent{});
    gCoordinator.addComponent(helmetEntity, ColliderComponent{});
    gCoordinator.addComponent(helmetEntity, AnimationComponent{});
    gCoordinator.addComponent(helmetEntity, ItemAnimationComponent{});
    gCoordinator.addComponent(helmetEntity, ItemComponent{.equipped = true});

    m_inventorySystem->pickUpItem(GameType::PickUpInfo{playerEntity, helmetEntity, GameType::slotType::HELMET});

    const Entity eventEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(
        eventEntity,
        SynchronisedEvent{
            .variant = SynchronisedEvent::Variant::REGISTER_ITEM, .entityID = helmet.id(), .entity = helmetEntity});
}

void Dungeon::update(const float deltaTime)
{
    m_itemSystem->update();
    m_playerMovementSystem->update(deltaTime);
    m_weaponSystem->update(deltaTime);
    m_animationSystem->update(deltaTime);
    m_roomListenerSystem->update(deltaTime);
    m_itemSpawnerSystem->updateAnimation(deltaTime);
    m_travellingSystem->update();
    m_passageSystem->update();
    m_characterSystem->update();
    m_textTagSystem->update(deltaTime);
    m_weaponBindSystem->update();
    m_dealDMGSystem->update();
    m_multiplayerSystem->update(deltaTime);

    for (const auto& remoteDungeonUpdate : m_multiplayerSystem->getRemoteDungeonUpdates())
    {
        switch (remoteDungeonUpdate.variant)
        {
        case MultiplayerDungeonUpdate::Variant::REGISTER_PLAYER:
            createRemotePlayer(remoteDungeonUpdate.player);
            break;
        case MultiplayerDungeonUpdate::Variant::CHANGE_ROOM:
            moveInDungeon(*remoteDungeonUpdate.room - m_currentPlayerPos, false);
            break;
        case MultiplayerDungeonUpdate::Variant::CHANGE_LEVEL:
            {
                auto& passageComponent = gCoordinator.getComponent<PassageComponent>(m_entities[m_id]);

                if (!passageComponent.activePassage) return;

                gCoordinator.getComponent<FloorComponent>(m_entities[m_id]).currentPlayerFloor += 1;
                passageComponent.activePassage = false;
                moveDownDungeon(false);
                break;
            }
        default:
            break;
        }
    }

    m_roomMap.at(m_currentPlayerPos).update();
    if (InputHandler::getInstance()->isPressed(InputType::ReturnInMenu))
    {
        m_stateChangeCallback({MenuStateMachine::StateAction::Pop}, {std::nullopt});
        return;
    }
    if (m_endGame)
    {
        m_stateChangeCallback({MenuStateMachine::StateAction::PutOnTop}, {std::make_unique<EndGameState>()});
        return;
    }
    checkForEndOfTheGame();
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
    m_floorGenerator.generateMainPath(6);
    m_floorGenerator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath(
        {.pathName{"BossCorridor"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath({.pathName{"BossRoom"},
                                       .startingPathName{"BossCorridor"},
                                       .endPathName{""},
                                       .minPathLength{0},
                                       .maxPathLength{0}});
    // m_floorGenerator.makeLockAndKey();

    m_roomMap = m_floorGenerator.getFloor(true);
    m_currentPlayerPos = m_floorGenerator.getStartingRoom();
    m_passageSystem->setPassages(m_floorGenerator.getEndingRoom() == m_currentPlayerPos);
    m_roomListenerSystem->changeRoom(m_currentPlayerPos);
}

inline void Dungeon::clearDungeon()
{
    m_doorSystem->clearDoors();
    m_spawnerSystem->clearSpawners();
    m_enemySystem->deleteEnemies();
    m_itemSpawnerSystem->deleteItems();
    m_chestSystem->deleteItems();
    m_collisionSystem->deleteMarkedBodies();
}

inline void Dungeon::loadMap(const std::string& path) const
{
    m_mapSystem->loadMap(path);
    m_textureSystem->loadTextures();
    m_collisionSystem->createMapCollision();
}

void Dungeon::moveDownDungeon(const bool createEvent)
{
    if (m_dungeonDepth >= configSingleton.GetConfig().maxDungeonDepth) m_endGame = true;
    ++m_dungeonDepth;

    makeSimpleFloor();
    const auto& pos = GameUtility::startingPosition;

    clearDungeon();
    m_passageSystem->setPassages(false);
    loadMap(m_roomMap.at(m_currentPlayerPos).getMap());

    b2Vec2 colliderPosition{};

    for (const auto& [id, player] : m_multiplayerSystem->getPlayers())
    {
        auto transformComponent = gCoordinator.getComponent<TransformComponent>(player);

        transformComponent.position = {getSpawnOffset(pos.x, id), getSpawnOffset(pos.y, id)};
        transformComponent.velocity = {};

        colliderPosition.x =
            transformComponent.position.x * static_cast<float>(configSingleton.GetConfig().pixelToMeterRatio);
        colliderPosition.y =
            transformComponent.position.y * static_cast<float>(configSingleton.GetConfig().pixelToMeterRatio);

        auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(player);
        colliderComponent.body->SetTransform(colliderPosition, colliderComponent.body->GetAngle());
    }

    m_roomListenerSystem->reset();

    if (createEvent)
    {
        const Entity eventEntity = gCoordinator.createEntity();
        gCoordinator.addComponent(eventEntity, SynchronisedEvent{.variant = SynchronisedEvent::Variant::LEVEL_CHANGED});
    }
}

void Dungeon::moveInDungeon(const glm::ivec2& dir, const bool createEvent)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir))
    {
        clearDungeon();
        m_currentPlayerPos += dir;
        m_roomListenerSystem->changeRoom(m_currentPlayerPos);

        loadMap(m_roomMap.at(m_currentPlayerPos).getMap());
        m_passageSystem->setPassages(m_currentPlayerPos == m_floorGenerator.getEndingRoom());


        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};

        b2Vec2 colliderPosition{};

        for (const auto& [id, player] : m_multiplayerSystem->getPlayers())
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(player);

            transformComponent.position = position + offset + GameType::MyVec2(id, id);
            transformComponent.velocity = {};

            colliderPosition.x = transformComponent.position.x * static_cast<float>(config::pixelToMeterRatio);
            colliderPosition.y = transformComponent.position.y * static_cast<float>(config::pixelToMeterRatio);

            auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(player);
            colliderComponent.body->SetTransform(colliderPosition, colliderComponent.body->GetAngle());
        }

        if (createEvent)
        {
            const Entity eventEntity = gCoordinator.createEntity();
            gCoordinator.addComponent(
                eventEntity,
                SynchronisedEvent{.variant = SynchronisedEvent::Variant::ROOM_CHANGED, .room = m_currentPlayerPos});
        }
    }
}

float Dungeon::getSpawnOffset(const float position, const uint32_t id)
{
    if (id % 2 == 0) return position + id * configSingleton.GetConfig().spawnOffset;
    return position - id * configSingleton.GetConfig().spawnOffset;
}

void Dungeon::checkForEndOfTheGame()
{
    for (const auto& [id, player] : m_multiplayerSystem->getPlayers())
    {
        if (gCoordinator.hasComponent<PlayerComponent>(player)) return;
    }
    m_stateChangeCallback({MenuStateMachine::StateAction::PutOnTop}, {std::make_unique<LostGameState>()});
}

void Dungeon::setECS()
{
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<SynchronisedEvent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<AnimationComponent>();
    gCoordinator.registerComponent<DoorComponent>();
    gCoordinator.registerComponent<TravellingDungeonComponent>();
    gCoordinator.registerComponent<SpawnerComponent>();
    gCoordinator.registerComponent<EnemyComponent>();
    gCoordinator.registerComponent<CharacterComponent>();
    gCoordinator.registerComponent<WeaponComponent>();
    gCoordinator.registerComponent<InventoryComponent>();
    gCoordinator.registerComponent<EquipmentComponent>();
    gCoordinator.registerComponent<TextTagComponent>();
    gCoordinator.registerComponent<PassageComponent>();
    gCoordinator.registerComponent<FloorComponent>();
    gCoordinator.registerComponent<LootComponent>();
    gCoordinator.registerComponent<ItemComponent>();
    gCoordinator.registerComponent<ItemAnimationComponent>();
    gCoordinator.registerComponent<ChestComponent>();
    gCoordinator.registerComponent<HelmetComponent>();
    gCoordinator.registerComponent<PotionComponent>();
    gCoordinator.registerComponent<BodyArmourComponent>();
    gCoordinator.registerComponent<WeaponSwingComponent>();
    gCoordinator.registerComponent<BindSwingWeaponEvent>();
    gCoordinator.registerComponent<DealDMGToEnemyEvent>();

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
        signature.set(gCoordinator.getComponentType<SynchronisedEvent>());
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
        signature.set(gCoordinator.getComponentType<PlayerComponent>());
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

    const auto itemSystem = gCoordinator.getRegisterSystem<ItemSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<ItemComponent>());
        gCoordinator.setSystemSignature<ItemSystem>(signature);
    }

    const auto weaponBindSystem = gCoordinator.getRegisterSystem<WeaponBindSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<BindSwingWeaponEvent>());
        gCoordinator.setSystemSignature<WeaponBindSystem>(signature);
    }

    const auto dealDMGToEnemySystem = gCoordinator.getRegisterSystem<DealDMGToEnemySystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<DealDMGToEnemyEvent>());
        gCoordinator.setSystemSignature<DealDMGToEnemySystem>(signature);
    }

    m_dealDMGSystem = gCoordinator.getRegisterSystem<DealDMGToEnemySystem>().get();
    m_weaponBindSystem = gCoordinator.getRegisterSystem<WeaponBindSystem>().get();
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
    m_inventorySystem = gCoordinator.getRegisterSystem<InventorySystem>().get();
    m_collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>().get();
    m_chestSystem = gCoordinator.getRegisterSystem<ChestSystem>().get();
    m_roomListenerSystem = gCoordinator.getRegisterSystem<RoomListenerSystem>().get();
    m_itemSpawnerSystem = gCoordinator.getRegisterSystem<ItemSpawnerSystem>().get();
    m_itemSystem = gCoordinator.getRegisterSystem<ItemSystem>().get();
}
