#include "Dungeon.h"

#include "Coordinator.h"

#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "DoorComponent.h"
#include "DoorSystem.h"
#include "Helpers.h"
#include "InputHandler.h"
#include "MapComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

#include "MapSystem.h"
#include "PlayerMovementSystem.h"
#include "TextureSystem.h"

#include "AnimationSystem.h"
#include "Paths.h"

extern Coordinator gCoordinator;

void Dungeon::init()
{
    setECS();

    m_entities[0] = gCoordinator.createEntity();
    m_entities[1] = gCoordinator.createEntity();
    const auto texture = new sf::Texture();
    const auto texture2 = new sf::Texture();
    const std::string PathToAssets{ASSET_PATH};
    texture->loadFromFile(PathToAssets + "/knight/knight.png");
    texture2->loadFromFile(PathToAssets + "/knight/knight.png");

    gCoordinator.addComponent(m_entities[0], RenderComponent{.sprite = std::move(sf::Sprite(*texture)), .layer = 4});
    gCoordinator.addComponent(m_entities[0], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[0], AnimationComponent{});
    gCoordinator.addComponent(m_entities[0], PlayerComponent{});
    gCoordinator.addComponent(m_entities[0], ColliderComponent{});

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[0], "FirstPlayer", {},
        [&](const GameType::CollisionData& entityT)
        {
            if (entityT.tag == "Door")
            {
                const auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entityT.entityID);

                m_moveInDungeon.emplace_back(GameType::mapDoorsToGeo.at(doorComponent.entrance));
            }
        },
        false, true);

    gCoordinator.addComponent(m_entities[1], RenderComponent{.sprite = std::move(sf::Sprite(*texture2)), .layer = 4});
    gCoordinator.addComponent(m_entities[1], TransformComponent(sf::Vector2f(50.f, 50.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[1], AnimationComponent{});
    gCoordinator.addComponent(m_entities[1], ColliderComponent{});
    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        m_entities[1], "SecondPlayer", {}, [](const GameType::CollisionData& entityT) {}, true, true);

    makeSimpleFloor();

    m_roomMap.at(m_currentPlayerPos).init();

    auto mapPath = m_roomMap.at(m_currentPlayerPos).getMap();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(mapPath);
}

void Dungeon::draw() const
{
    gCoordinator.getRegisterSystem<TextureSystem>()->loadTextures();
    gCoordinator.getRegisterSystem<AnimationSystem>()->updateFrames();
    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::update()
{
    gCoordinator.getRegisterSystem<PlayerMovementSystem>()->update();
    m_roomMap.at(m_currentPlayerPos).update();

    if (!m_moveInDungeon.empty())
    {
        const auto tmpMove = m_moveInDungeon.front();
        m_moveInDungeon.pop_front();
        moveInDungeon(tmpMove);
    }

    counter++;
    if (m_passedBy && counter > (30))
    {
        m_passedBy = false;
        counter = 0;
    }
}

void Dungeon::setECS()
{
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<AnimationComponent>();
    gCoordinator.registerComponent<DoorComponent>();

    auto playerMovementSystem = gCoordinator.getRegisterSystem<PlayerMovementSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<PlayerComponent>());
        gCoordinator.setSystemSignature<PlayerMovementSystem>(signature);
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

    const auto textureSystem = gCoordinator.getRegisterSystem<TextureSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        signature.set(gCoordinator.getComponentType<TileComponent>());
        gCoordinator.setSystemSignature<TextureSystem>(signature);
    }

    const auto animationSystem = gCoordinator.getRegisterSystem<AnimationSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<TileComponent>());
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        gCoordinator.setSystemSignature<AnimationSystem>(signature);
    }

    textureSystem->loadTexturesFromFiles();

    for (int i = 1000; i < 1800; i++)
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
    if (m_roomMap.contains(m_currentPlayerPos + dir) && !m_passedBy)
    {
        m_currentPlayerPos += dir;
        std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();
        gCoordinator.getRegisterSystem<DoorSystem>()->clearDoors();
        gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
        const auto newDoor = dir * -1;
        const auto doorType = GameType::geoToMapDoors.at(newDoor);
        const auto position = gCoordinator.getRegisterSystem<DoorSystem>()->getDoorPosition(doorType);
        const auto offset = glm::vec2{dir.x * 100, -dir.y * 100};
        const sf::Vector2f newPosition = {position.x + offset.x, position.y + offset.y};
        gCoordinator.getComponent<TransformComponent>(m_entities[0]).position = newPosition;
        auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(m_entities[0]);
        colliderComponent.body->SetTransform(
            {convertPixelsToMeters(newPosition.x), convertPixelsToMeters(newPosition.y)},
            colliderComponent.body->GetAngle());
        m_passedBy = true;
        counter = 0;
    }
}
