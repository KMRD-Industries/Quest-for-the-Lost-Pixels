#include "Game.h"

#include <TextureSystem.h>

#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "FightActionEvent.h"
#include "FightSystem.h"
#include "ObjectCreatorSystem.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void Game::init()
{
    gCoordinator.init();

    gCoordinator.registerComponent<ColliderComponent>();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TransformComponent>();
    gCoordinator.registerComponent<CreateBodyWithCollisionEvent>();
    gCoordinator.registerComponent<FightActionEvent>();

    auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        signature.set(gCoordinator.getComponentType<ColliderComponent>());
        gCoordinator.setSystemSignature<CollisionSystem>(signature);
    }

    auto renderSystem = gCoordinator.getRegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<RenderComponent>());
        signature.set(gCoordinator.getComponentType<TransformComponent>());
        gCoordinator.setSystemSignature<RenderSystem>(signature);
    }

    auto objectCreatorSystem = gCoordinator.getRegisterSystem<ObjectCreatorSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<CreateBodyWithCollisionEvent>());
        gCoordinator.setSystemSignature<ObjectCreatorSystem>(signature);
    }

    auto fightSystem = gCoordinator.getRegisterSystem<FightSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<FightActionEvent>());
        gCoordinator.setSystemSignature<FightSystem>(signature);
    }

    m_collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>().get();
    m_renderSystem = gCoordinator.getRegisterSystem<RenderSystem>().get();
    m_objectCreatorSystem = gCoordinator.getRegisterSystem<ObjectCreatorSystem>().get();
    m_fightSystem = gCoordinator.getRegisterSystem<FightSystem>().get();

    m_dungeon.init();
}

void Game::draw() { m_dungeon.draw(); }

void Game::update(const float deltaTime)
{
    m_objectCreatorSystem->update();
    m_fightSystem->update();
    m_dungeon.update(deltaTime);
    m_collisionSystem->deleteMarkedBodies();
}

void Game::handleCollision()
{
    m_collisionSystem->update();
    constexpr auto timeStep = 1.f / 60.f;
    m_collisionSystem->updateSimulation(timeStep, 8, 3);
};

std::string Game::getBackground() { return gCoordinator.getRegisterSystem<TextureSystem>()->getBackgroundColor(); }