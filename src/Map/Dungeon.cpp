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
#include "FloorComponent.h"
#include "GameUtility.h"
#include "PassageComponent.h"
#include "PassageSystem.h"
#include "SpawnerComponent.h"
#include "SpawnerSystem.h"

extern Coordinator gCoordinator;

void Dungeon::init()
{
    setECS();
    gCoordinator.getRegisterSystem<TextureSystem>()->loadTexturesFromFiles();

    m_id = config::playerEntity;
    const Entity player = gCoordinator.createEntity();
    const auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    multiplayerSystem->setup("127.0.0.1", "9001");

    if (multiplayerSystem->isConnected())
    {
        m_id = multiplayerSystem->registerPlayer(player);
        std::cout << "Connected to server with id: {}" << m_id;
    }
    else
    {
        std::cout << "Starting in single-player mode";
    }

    makeStartFloor();
    m_roomMap.at(m_currentPlayerPos).init();

    auto mapPath = m_roomMap.at(m_currentPlayerPos).getMap();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(mapPath);
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

    m_entities[m_id] = player;

    gCoordinator.addComponent(m_entities[m_id], TileComponent{config::playerAnimation, "Characters", 3});
    gCoordinator.addComponent(m_entities[m_id], RenderComponent{});
    gCoordinator.addComponent(m_entities[m_id], TransformComponent{startingPosition});
    gCoordinator.addComponent(m_entities[m_id], AnimationComponent{});
    gCoordinator.addComponent(m_entities[m_id], CharacterComponent{.hp = 100.f});
    gCoordinator.addComponent(m_entities[m_id], PlayerComponent{});
    gCoordinator.addComponent(m_entities[m_id], ColliderComponent{});
    gCoordinator.addComponent(m_entities[m_id], FloorComponent{});
    gCoordinator.addComponent(
        m_entities[m_id],
        TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) { moveInDungeon(dir); }});
    gCoordinator.addComponent(m_entities[m_id], PassageComponent{.moveCallback = [this]() { moveDownDungeon(); }});
    ;

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", config::playerAnimation);
    gCoordinator.getComponent<ColliderComponent>(m_entities[0]).collision = cc;

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[m_id], "Player 1", {cc.width, cc.height},
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

            if (entityT.tag == "Passage")
            {
                auto& passageComponent = gCoordinator.getComponent<PassageComponent>(m_entities[m_id]);

                if (!passageComponent.activePassage) return;

                gCoordinator.getComponent<FloorComponent>(m_entities[m_id]).currentPlayerFloor += 1;
                passageComponent.moveInDungeon.emplace_back(true);
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

            if (entityT.tag == "Passage")
            {
            }
        },
        false, false, {cc.x, cc.y});
}

void Dungeon::draw() const { gCoordinator.getRegisterSystem<TextureSystem>()->loadTextures(); }

void Dungeon::update()
{
    gCoordinator.getRegisterSystem<PlayerMovementSystem>()->update();
    gCoordinator.getRegisterSystem<SpawnerSystem>()->update();
    gCoordinator.getRegisterSystem<EnemySystem>()->update();
    gCoordinator.getRegisterSystem<TravellingSystem>()->update();
    gCoordinator.getRegisterSystem<PassageSystem>()->update();
    gCoordinator.getRegisterSystem<CharacterSystem>()->update();
    gCoordinator.getRegisterSystem<AnimationSystem>()->updateFrames();
    gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(m_floorGenerator.getEndingRoom() ==
                                                                 m_currentPlayerPos);

    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();

    if (multiplayerSystem->isConnected())
    {
        constexpr int playerAnimationTile = 243;
        auto stateUpdate = multiplayerSystem->pollStateUpdates();
        std::uint32_t id = stateUpdate.id();
        auto tag = std::format("Player {}", id);

        switch (stateUpdate.variant())
        {
        case comm::DISCONNECTED:
            gCoordinator.destroyEntity(m_entities[id]);
            multiplayerSystem->entityDisconnected(id);
            break;
        case comm::CONNECTED:
            m_entities[id] = gCoordinator.createEntity();

            gCoordinator.addComponent(m_entities[id], TileComponent{playerAnimationTile, "Characters", 4});
            gCoordinator.addComponent(m_entities[id], RenderComponent{});
            gCoordinator.addComponent(m_entities[id], TransformComponent{});
            gCoordinator.addComponent(m_entities[id], AnimationComponent{});
            gCoordinator.addComponent(m_entities[id], ColliderComponent{});
            gCoordinator.addComponent(m_entities[id], CharacterComponent{.hp = 100.f});

            gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(m_entities[id], tag);

            multiplayerSystem->entityConnected(id, m_entities[id]);
            break;
        default:
            break;
        }

        multiplayerSystem->update();
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
}

void Dungeon::makeStartFloor()
{
    Room room{0};
    m_roomMap.clear();
    m_roomMap.emplace(glm::ivec2{0, 0}, room);
    m_currentPlayerPos = {0.f, 0.f};

    gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(true);
}

void Dungeon::makeSimpleFloor()
{
    m_floorGenerator.setFloorID(gCoordinator.getComponent<FloorComponent>(config::playerEntity).currentPlayerFloor);

    m_floorGenerator.generateFloor(5, 6);
    m_floorGenerator.generateMainPath(11);
    m_floorGenerator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    m_floorGenerator.makeLockAndKey();

    m_roomMap = m_floorGenerator.getFloor(true);
    m_currentPlayerPos = m_floorGenerator.getStartingRoom();

    gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(m_floorGenerator.getEndingRoom() ==
                                                                 m_currentPlayerPos);
}

void Dungeon::moveDownDungeon()
{
    makeSimpleFloor();
    const std::string& newMap = m_roomMap.at(m_currentPlayerPos).getMap();

    gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
    gCoordinator.getRegisterSystem<SpawnerSystem>()->clearSpawners();
    gCoordinator.getRegisterSystem<EnemySystem>()->deleteEnemies();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
    gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(false);

    gCoordinator.getComponent<TransformComponent>(m_entities[0]).position = startingPosition;
    gCoordinator.getComponent<TransformComponent>(m_entities[0]).velocity = {};

    b2Vec2 position = gCoordinator.getComponent<ColliderComponent>(m_entities[0]).body->GetPosition();
    position.x = startingPosition.x * static_cast<float>(config::pixelToMeterRatio);
    position.y = startingPosition.y * static_cast<float>(config::pixelToMeterRatio);

    gCoordinator.getComponent<ColliderComponent>(m_entities[0]).body->SetTransform(position, 0);
    gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();
}

void Dungeon::moveInDungeon(const glm::ivec2& dir)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir))
    {
        const std::uint32_t id = gCoordinator.getRegisterSystem<MultiplayerSystem>()->playerID();
        m_currentPlayerPos += dir;
        const std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();

        gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
        gCoordinator.getRegisterSystem<SpawnerSystem>()->clearSpawners();
        gCoordinator.getRegisterSystem<EnemySystem>()->deleteEnemies();
        gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
        gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(m_currentPlayerPos ==
                                                                     m_floorGenerator.getEndingRoom());
        gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};

        const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};
        gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;

        const auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);

        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
    }
}
