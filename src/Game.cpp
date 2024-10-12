#include "Game.h"

#include "ButtonSystem.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "MainMenuState.h"
#include "RenderSystem.h"

extern Coordinator gCoordinator;

void Game::init()
{
    m_stateManager.handleStateChange({MenuStateMachine::StateAction::Push}, {std::make_unique<MainMenuState>()});
}

void Game::draw(sf::RenderWindow& window)
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->render();
    m_stateManager.render(window);
}

void Game::update(const float deltaTime)
{
    m_stateManager.update(deltaTime);
}