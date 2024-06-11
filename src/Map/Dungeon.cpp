#include <comm.pb.h>
#include <format>
#include <ostream>
#include <print>

#include "Dungeon.h"

#include "Coordinator.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "CharacterSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "DoorComponent.h"
#include "DoorSystem.h"
#include "Helpers.h"
#include "InputHandler.h"
#include "MapComponent.h"
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "TravellingDungeonComponent.h"

#include "MapSystem.h"
#include "PlayerMovementSystem.h"
#include "TextureSystem.h"
#include "TravellingSystem.h"

#include "Paths.h"

extern Coordinator gCoordinator;

void Dungeon::init()
{
    setECS();

    std::uint32_t id = 0;
    Entity player = gCoordinator.createEntity();
    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    multiplayerSystem->setup("127.0.0.1", "9001");

    if (multiplayerSystem->isConnected())
    {
        id = multiplayerSystem->registerPlayer(player);
        std::println("Connected to server with id: {}", id);
    }
    else
    {
        std::println("Starting in single-player mode");
    }


    m_entities[id] = player;
    const auto texture = new sf::Texture();
    texture->loadFromFile(m_asset_path + "/knight/knight.png");

    gCoordinator.addComponent(m_entities[id], RenderComponent{.sprite = std::move(sf::Sprite(*texture)), .layer = 4});
    gCoordinator.addComponent(m_entities[id], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[id], AnimationComponent{});
    gCoordinator.addComponent(m_entities[id], CharacterComponent{.hp = 100.f});
    gCoordinator.addComponent(m_entities[id], PlayerComponent{});
    gCoordinator.addComponent(m_entities[id], ColliderComponent{});
    gCoordinator.addComponent(m_entities[id], TravellingDungeonComponent{.moveCallback = [this](const glm::ivec2& dir) {
                                  moveInDungeon(dir);
                              }});

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[id], "Player 1", {},
        [&](const GameType::CollisionData& entityT)
        {
            if (entityT.tag == "Door")
            {
                const auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entityT.entityID);
                auto& travellingDungeonComponent =
                    gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[id]);

                if (travellingDungeonComponent.doorsPassed == 0)
                    travellingDungeonComponent.moveInDungeon.emplace_back(
                        GameType::mapDoorsToGeo.at(doorComponent.entrance));
                ++travellingDungeonComponent.doorsPassed;
            }
        },
        [&](const GameType::CollisionData& entityT)
        {
            if (entityT.tag == "Door")
            {
                auto& travellingDungeonComponent =
                    gCoordinator.getComponent<TravellingDungeonComponent>(m_entities[id]);
                --travellingDungeonComponent.doorsPassed;
            }
        },
        false, true);

    makeSimpleFloor();

    m_roomMap.at(m_currentPlayerPos).init();

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
    gCoordinator.getRegisterSystem<TravellingSystem>()->update();
    gCoordinator.getRegisterSystem<CharacterSystem>()->update();

    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();

    // if (false)
    if (multiplayerSystem->isConnected())
    {
        const auto texture = new sf::Texture();

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
            texture->loadFromFile(m_asset_path + "/knight/knight.png");

            std::cout << stateUpdate.ShortDebugString() << '\n';
            m_entities[id] = gCoordinator.createEntity();

            gCoordinator.addComponent(m_entities[id],
                                      RenderComponent{.sprite = std::move(sf::Sprite(*texture)), .layer = 4});
            gCoordinator.addComponent(m_entities[id],
                                      TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
            gCoordinator.addComponent(m_entities[id], AnimationComponent{});
            gCoordinator.addComponent(m_entities[id], ColliderComponent{});
            gCoordinator.addComponent(m_entities[id], CharacterComponent{.hp = 100.f});

            gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(m_entities[id], tag);

            multiplayerSystem->entityConnected(id, m_entities[id]);
            break;
        default:
            delete texture;
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
        gCoordinator.setSystemSignature<TextureSystem>(signature);
    }

    textureSystem->loadTexturesFromFiles();

    for (int i = 1000; i < 1500; i++)
    {
        m_entities[i] = gCoordinator.createEntity();
        gCoordinator.addComponent(m_entities[i], RenderComponent{});
        gCoordinator.addComponent(m_entities[i], TileComponent{});
        gCoordinator.addComponent(m_entities[i], TransformComponent{});
        gCoordinator.addComponent(m_entities[i], AnimationComponent{});
        gCoordinator.addComponent(m_entities[i], ColliderComponent{});
    }
}

void Dungeon::makeSimpleFloor()
{
    m_floorGenerator.generateFloor(5, 6);
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
        std::uint32_t id = gCoordinator.getRegisterSystem<MultiplayerSystem>()->playerID();
        m_currentPlayerPos += dir;
        std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();
        gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
        gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
        gCoordinator.getRegisterSystem<CollisionSystem>()->createMapCollision();

        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};
        const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};
        gCoordinator.getComponent<TransformComponent>(m_entities[id]).position = newPosition;
        auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[id]);
        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
    }
}
