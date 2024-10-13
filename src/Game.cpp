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
    m_stateManager.handleStateChange(MenuStateMachine::StateAction::Push, std::make_unique<MainMenuState>());
}

void Game::draw(sf::RenderWindow& window)
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->render();
    m_stateManager.render(window);
}

void Game::draw() { m_dungeon.draw(); }

void Game::update(const float deltaTime)
{
    m_stateManager.update(deltaTime);
}

void Game::handleCollision()
{
    m_collisionSystem->update();
    constexpr auto timeStep = 1.f / 60.f;
    m_collisionSystem->updateSimulation(timeStep, 8, 3);
};

std::string Game::getBackground() { return gCoordinator.getRegisterSystem<TextureSystem>()->getBackgroundColor(); }