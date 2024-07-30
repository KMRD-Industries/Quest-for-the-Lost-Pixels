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
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "EquipWeaponSystem.h"
#include "EquippedWeaponComponent.h"
#include "InventoryComponent.h"
#include "InventorySystem.h"
#include "MapComponent.h"
#include "MapSystem.h"
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "PlayerMovementSystem.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "SpawnerSystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "TravellingDungeonComponent.h"
#include "TravellingSystem.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;

void Dungeon::init()
{
    setECS();

    m_id = 0;
    const Entity player = gCoordinator.createEntity();
    auto multiplayerSystem = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    multiplayerSystem->setup("127.0.0.1", "9001");

    if (multiplayerSystem->isConnected())
    {
        m_id = multiplayerSystem->registerPlayer(player);
        std::cout << "Connected to server with id: {" << m_id << "}";
    }
    else
    {
        std::cout << "Starting in single-player mode";
    }

    constexpr int playerAnimationTile = 243;

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
    gCoordinator.addComponent(m_entities[m_id], InventoryComponent{});
    gCoordinator.addComponent(m_entities[m_id], EquippedWeaponComponent{});

    Collision cc = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision("Characters", 185);
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

    const Entity weaponEntity = gCoordinator.createEntity();
    gCoordinator.addComponent(weaponEntity, WeaponComponent{.weaponID = 147});
    gCoordinator.addComponent(weaponEntity, TileComponent{148, "Weapons", 5});
    gCoordinator.addComponent(weaponEntity, TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(weaponEntity, RenderComponent{});
    gCoordinator.addComponent(weaponEntity, ColliderComponent{});
    gCoordinator.addComponent(weaponEntity, AnimationComponent{});

    gCoordinator.getRegisterSystem<InventorySystem>()->pickUpWeapon(m_entities[m_id], weaponEntity);
    gCoordinator.getRegisterSystem<EquipWeaponSystem>()->equipWeapon(m_entities[m_id], weaponEntity);

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
    gCoordinator.getRegisterSystem<SpawnerSystem>()->update();
    gCoordinator.getRegisterSystem<EnemySystem>()->update();
    gCoordinator.getRegisterSystem<TravellingSystem>()->update();
    gCoordinator.getRegisterSystem<CharacterSystem>()->update();
    gCoordinator.getRegisterSystem<AnimationSystem>()->updateFrames();

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
            gCoordinator.addComponent(m_entities[id],
                                      TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
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
    gCoordinator.registerComponent<WeaponComponent>();
    gCoordinator.registerComponent<InventoryComponent>();
    gCoordinator.registerComponent<EquippedWeaponComponent>();

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

    const auto equipWeaponSystem = gCoordinator.getRegisterSystem<EquipWeaponSystem>();
    {
    }

    const auto inventorySystem = gCoordinator.getRegisterSystem<InventorySystem>();
    {
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
        gCoordinator.getRegisterSystem<SpawnerSystem>()->clearSpawners();
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
