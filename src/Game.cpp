#include "Game.h"

#include <imgui.h>

#include "ButtonComponent.h"
#include "ButtonSystem.h"
#include  "ButtonSystem.h";
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "MainMenuState.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void Game::init()
{
    gCoordinator.init();

    gCoordinator.registerComponent<ButtonComponent>();
    gCoordinator.registerComponent<ColliderComponent>();
    gCoordinator.registerComponent<RenderComponent>();
    gCoordinator.registerComponent<TransformComponent>();

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

    auto buttonSystem = gCoordinator.getRegisterSystem<ButtonSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<ButtonComponent>());
        gCoordinator.setSystemSignature<ButtonSystem>(signature);
    }

    m_stateManager.handleStateChange(MenuStateMachine::StateAction::Push, std::make_unique<MainMenuState>());
}

void Game::draw()
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->render();
    m_stateManager.render();
}

void Game::update(const float deltaTime)
{
    m_stateManager.update(deltaTime);
    gCoordinator.getRegisterSystem<CollisionSystem>()->deleteMarkedBodies();
}

void Game::handleCollision()
{
    const auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    collisionSystem->updateCollision();
    constexpr auto timeStep = 1.f / 60.f;
    collisionSystem->updateSimulation(timeStep, 8, 3);
};