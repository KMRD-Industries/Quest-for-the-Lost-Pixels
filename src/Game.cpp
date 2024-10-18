#include "Game.h"

#include "ButtonSystem.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "MainMenuState.h"
#include "Paths.h"
#include "RenderSystem.h"
#include "SoundComponent.h"
#include "SoundSystem.h"

extern Coordinator gCoordinator;

void Game::init()
{
    m_stateManager.handleStateChange({MenuStateMachine::StateAction::Push}, {std::make_unique<MainMenuState>()});
    auto backgroundMusic = gCoordinator.createEntity();
    gCoordinator.addComponent(backgroundMusic, SoundComponent{ASSET_PATH + std::string("/sounds/backgroundSound.mp3")});
}

void Game::draw(sf::RenderWindow& window)
{
    gCoordinator.getRegisterSystem<ButtonSystem>()->render();
    m_stateManager.render(window);
}

void Game::update(const float deltaTime)
{
    gCoordinator.getRegisterSystem<SoundSystem>()->update(deltaTime);
    m_stateManager.update(deltaTime);
}