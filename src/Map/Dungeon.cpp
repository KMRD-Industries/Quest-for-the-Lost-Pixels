#include <chrono>
#include <format>
#include <print>

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
#include "MapComponent.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "TravellingDungeonComponent.h"
#include "TravellingSystem.h"

#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "SpawnerComponent.h"
#include "SpawnerSystem.h"
#include "glm/ext/vector_int2.hpp"

extern Coordinator gCoordinator;

void Dungeon::init()
{
    setECS();

    m_id = 0;
    m_seed = std::chrono::system_clock::now().time_since_epoch().count();
    const Entity player = gCoordinator.createEntity();

    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    multiplayerSystem->setup("127.0.0.1", "9001");

    if (multiplayerSystem->isConnected())
    {
        auto gameState = multiplayerSystem->registerPlayer(player);
        m_id = gameState.player_id();
        m_seed = gameState.seed();

        std::println("Connected to server with id: {}", m_id);

        for (uint32_t id : gameState.connected_players())
        {
            createRemotePlayer(id);
            std::println("Connected remote player {}", id);
        }
    }
    else
    {
        std::cout << "Starting in single-player mode";
    }

    constexpr int playerAnimationTile = 185;
    const std::string tag = std::format("Player {}", m_id);

    m_entities[m_id] = player;

    gCoordinator.addComponent(m_entities[m_id], TileComponent{playerAnimationTile, "Characters", 4});
    gCoordinator.addComponent(m_entities[m_id], RenderComponent{});
    gCoordinator.addComponent(m_entities[m_id],
                              TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f), {0.f, 0.f}));
    gCoordinator.addComponent(m_entities[m_id], AnimationComponent{});
    gCoordinator.addComponent(m_entities[m_id], CharacterComponent{.hp = 100.f});
    gCoordinator.addComponent(m_entities[m_id], PlayerComponent{});
    gCoordinator.addComponent(m_entities[m_id], ColliderComponent{});
    gCoordinator.addComponent(
        m_entities[m_id],
        TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) { moveInDungeon(dir); }});

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", playerAnimationTile);
    gCoordinator.getComponent<ColliderComponent>(m_entities[m_id]).collision = cc;

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[m_id], tag, {cc.width, cc.height},
        [&](const GameType::CollisionData& entityT)
        {
            if (entityT.tag == "Door")
            {
                const auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entityT.entityID);
                auto& travellingDungeonComponent =
                    gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[m_id]);

                if (travellingDungeonComponent.doorsPassed == 0)
                {
                    travellingDungeonComponent.moveInDungeon.emplace_back(
                        GameType::mapDoorsToGeo.at(doorComponent.entrance));
                }
                ++travellingDungeonComponent.doorsPassed;
            }
        },
        [&](const GameType::CollisionData& entityT)
        {
            if (entityT.tag == "Door")
            {
                auto& travellingDungeonComponent =
                    gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[m_id]);
                --travellingDungeonComponent.doorsPassed;
            }
        },
        false, false, {cc.x, cc.y});

    makeSimpleFloor();

    m_roomMap.at(m_currentPlayerPos).init();
    multiplayerSystem->setRoom(m_currentPlayerPos);
    m_players.insert(m_id);

    auto mapPath = m_roomMap.at(m_currentPlayerPos).getMap();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(mapPath);
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();
}

void Dungeon::draw() const
{
    gCoordinator.getRegisterSystem<TextureSystem>()->loadTextures();

    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::update()
{
    gCoordinator.getRegisterSystem<PlayerMovementSystem>()->update();
    gCoordinator.getRegisterSystem<SpawnerSystem>()->update();
    gCoordinator.getRegisterSystem<EnemySystem>()->update();
    gCoordinator.getRegisterSystem<TravellingSystem>()->update();
    gCoordinator.getRegisterSystem<CharacterSystem>()->update();
    gCoordinator.getRegisterSystem<AnimationSystem>()->updateFrames();

    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();

    if (multiplayerSystem->isConnected())
    {
        auto stateUpdate = multiplayerSystem->pollStateUpdates();
        uint32_t id = stateUpdate.id();

        switch (stateUpdate.variant())
        {
        case comm::DISCONNECTED:
            gCoordinator.destroyEntity(m_entities[id]);
            multiplayerSystem->entityDisconnected(id);
            m_players.erase(id);
            break;
        case comm::CONNECTED:
            createRemotePlayer(id);
            m_players.insert(id);
            break;
        case comm::ROOM_CHANGED:
            changeRoom(multiplayerSystem->getRoom());
            break;
        default:
            break;
        }

        multiplayerSystem->update();
    }


    m_roomMap.at(m_currentPlayerPos).update();
}

void Dungeon::createRemotePlayer(uint32_t id)
{
    constexpr int playerAnimationTile = 185;
    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    auto tag = std::format("Player {}", id);

    m_entities[id] = gCoordinator.createEntity();

    gCoordinator.addComponent(m_entities[id], TileComponent{playerAnimationTile, "Characters", 4});
    gCoordinator.addComponent(m_entities[id], RenderComponent{});
    gCoordinator.addComponent(m_entities[id], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[id], AnimationComponent{});
    gCoordinator.addComponent(m_entities[id], ColliderComponent{});
    gCoordinator.addComponent(m_entities[id], CharacterComponent{.hp = 100.f});

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", playerAnimationTile);
    gCoordinator.getComponent<ColliderComponent>(m_entities[id]).collision = cc;

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[id], tag, {cc.width, cc.height}, [&](const GameType::CollisionData& entityT) {},
        [&](const GameType::CollisionData& entityT) {}, false, false, {cc.x, cc.y});

    multiplayerSystem->entityConnected(id, m_entities[id]);
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
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<MapComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    auto doorSystem = gCoordinator.getRegisterSystem<DoorSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<DoorComponent>());
        gCoordinator.setSystemSignature<DoorSystem>(signature);
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
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
        gCoordinator.setSystemSignature<TextureSystem>(signature);
    }

    const auto animationSystem = gCoordinator.getRegisterSystem<AnimationSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<TextureSystem>(signature);
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


    textureSystem->loadTexturesFromFiles();

    for (int i = config::mapFirstEntity; i < config::mapFirstEntity + config::numberOfMapEntities; i++)
    {
        m_entities[i] = gCoordinator.createEntity();
        gCoordinator.addComponent(m_entities[i], RenderComponent{});
        gCoordinator.addComponent(m_entities[i], TileComponent{});
        gCoordinator.addComponent(m_entities[i], TransformComponent{});
        gCoordinator.addComponent(m_entities[i], AnimationComponent{});
        gCoordinator.addComponent(m_entities[i], ColliderComponent{});
        gCoordinator.addComponent(m_entities[i], MapComponent{});
    }

    for (int i = config::enemyFirstEntity; i < config::enemyFirstEntity + config::numberOfEnemyEntities; i++)
    {
        m_entities[i] = gCoordinator.createEntity();
        gCoordinator.addComponent(m_entities[i], RenderComponent{});
        gCoordinator.addComponent(m_entities[i], TileComponent{});
        gCoordinator.addComponent(m_entities[i], TransformComponent{});
        gCoordinator.addComponent(m_entities[i], AnimationComponent{});
        gCoordinator.addComponent(m_entities[i], ColliderComponent{});
        gCoordinator.addComponent(m_entities[i], EnemyComponent{});
        gCoordinator.addComponent(m_entities[i], CharacterComponent{.hp = 10.f});
    }
}

void Dungeon::makeSimpleFloor()
{
    m_floorGenerator.generateFloor(5, 6, m_seed);
    m_floorGenerator.generateMainPath(11);
    m_floorGenerator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.makeLockAndKey();

    m_roomMap = m_floorGenerator.getFloor(true);
    m_currentPlayerPos = m_floorGenerator.getStartingRoom();
}

void Dungeon::moveInDungeon(const glm::ivec2& dir)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir))
    {
        m_currentPlayerPos += dir;
        std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();

        gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
        gCoordinator.getRegisterSystem<SpawnerSystem>()->clearSpawners();
        gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
        gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};
        const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};

        for (auto id : m_players)
        {
            gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;
            auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);
            colliderComponent.body->SetTransform(
                {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
                colliderComponent.body->GetAngle());
        }


        auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
        if (multiplayerSystem->isConnected()) multiplayerSystem->roomChanged(m_currentPlayerPos);
    }
}

void Dungeon::changeRoom(const glm::ivec2& room)
{
    auto dir = room - m_currentPlayerPos;
    m_currentPlayerPos = room;
    std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();
    gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
    gCoordinator.getRegisterSystem<SpawnerSystem>()->clearSpawners();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

    const auto newDoor = dir * -1;
    const auto doorType = GameType::geoToMapDoors.at(newDoor);
    const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
    const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};
    const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};

    for (auto id : m_players)
    {
        gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;
        auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);
        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
    }
}
