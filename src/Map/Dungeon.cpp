#include "Dungeon.h"

#include "Coordinator.h"

#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "InputHandler.h"
#include "MapComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TileComponent.h"
#include "TransformComponent.h"

#include "MapSystem.h"
#include "PlayerMovementSystem.h"
#include "RenderSystem.h"
#include "TextureSystem.h"

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
    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(m_entities[0], {}, false, true);

    gCoordinator.addComponent(m_entities[1], RenderComponent{.sprite = std::move(sf::Sprite(*texture2)), .layer = 4});
    gCoordinator.addComponent(m_entities[1], TransformComponent(sf::Vector2f(50.f, 50.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[1], AnimationComponent{});
    gCoordinator.addComponent(m_entities[1], ColliderComponent{});
    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(m_entities[1], {}, true, true);

    makeSimpleFloor();

    m_roomMap.at(m_currentPlayerPos).init();

    auto mapPath = m_roomMap.at(m_currentPlayerPos).getMap();
    gCoordinator.getRegisterSystem<MapSystem>()->loadMap(mapPath);
}

void Dungeon::draw() const
{
    gCoordinator.getRegisterSystem<TextureSystem>()->loadTextures();

    m_roomMap.at(m_currentPlayerPos).draw();
}

void Dungeon::update()
{
    gCoordinator.getRegisterSystem<PlayerMovementSystem>()->update();

    m_roomMap.at(m_currentPlayerPos).update();

    const auto inputHandler{InputHandler::getInstance()};

    glm::ivec2 dir{0, 0};
    if (inputHandler->isPressed(InputType::MoveUp)) // Move Up
        dir += glm::ivec2{0, 1};
    else if (inputHandler->isPressed(InputType::MoveDown)) // Move Down
        dir -= glm::ivec2{0, 1};
    else if (inputHandler->isPressed(InputType::MoveRight)) // Move Right
        dir += glm::ivec2{1, 0};
    else if (inputHandler->isPressed(InputType::MoveLeft)) // Move Left
        dir -= glm::ivec2{1, 0};

    if (dir != glm::ivec2{0, 0} && m_floorGenerator.isConnected(m_currentPlayerPos, m_currentPlayerPos + dir))
    {
        m_currentPlayerPos += dir;
        std::string newMap = m_roomMap.at(m_currentPlayerPos).getMap();
        gCoordinator.getRegisterSystem<MapSystem>()->loadMap(newMap);
    }
}

void Dungeon::setECS()
{
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<AnimationComponent>();

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
        signature.set(gCoordinator.getComponentType<AnimationComponent>());
        gCoordinator.setSystemSignature<MapSystem>(signature);
    }

    auto textureSystem = gCoordinator.getRegisterSystem<TextureSystem>();
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

void Dungeon::moveInDungeon(glm::ivec2 dir)
{
    if (m_roomMap.contains(m_currentPlayerPos + dir)) m_currentPlayerPos += dir;
}
