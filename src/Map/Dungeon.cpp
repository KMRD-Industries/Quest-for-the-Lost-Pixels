#include "Dungeon.h"

#include "Coordinator.h"

#include "AnimationComponent.h"
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
    sf::Texture* texture = new sf::Texture();
    std::string PathToAssets{ASSET_PATH};
    texture->loadFromFile(PathToAssets + "/knight/knight.png");

    gCoordinator.addComponent(m_entities[0], RenderComponent{.sprite = std::move(sf::Sprite(*texture)), .layer = 4});
    gCoordinator.addComponent(m_entities[0], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));
    gCoordinator.addComponent(m_entities[0], AnimationComponent{});
    gCoordinator.addComponent(m_entities[0], PlayerComponent{});

    gCoordinator.addComponent(m_entities[1], RenderComponent{.sprite = sf::Sprite(*texture)});
    gCoordinator.addComponent(m_entities[1], TransformComponent(sf::Vector2f(0.f, 0.f), 0.f, sf::Vector2f(1.f, 1.f)));

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
}

void Dungeon::setECS()
{
    gCoordinator.init();
    gCoordinator.registerComponent<MapComponent>();
    gCoordinator.registerComponent<PlayerComponent>();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TileComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<AnimationComponent>();

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

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

    m_roomMap = m_floorGenerator.getFloor();
}
