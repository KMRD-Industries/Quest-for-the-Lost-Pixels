#include "MainMenuState.h"

#include "ButtonDTO.h"
#include "GameState.h"
#include "Paths.h"

void MainMenuState::init()
{
    m_playButton.init(ButtonDTO{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                ASSET_PATH + std::string("/ui/uiFont.ttf"), "Start",
                                40.f, 2.f,
                                {818, 850}, {0, 387, 144, 42},
                                {144, 387, 144, 42}, {288, 387, 144, 42},
                                {0, 0}, {1, 1}, [this]()
                                {
                                    m_stateChangeCallback(
                                        MenuStateMachine::StateAction::Push,
                                        std::make_unique<GameState>());
                                }});

    m_quitButton.init(ButtonDTO{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                ASSET_PATH + std::string("/ui/uiFont.ttf"), "Quit",
                                40.f, 2.f,
                                {818, 950}, {0, 387, 144, 42},
                                {144, 387, 144, 42}, {288, 387, 144, 42},
                                {0, 0}, {1, 1}, []()
                                {
                                    std::exit(1);
                                }
    });

    m_backgroundGraphics.init(std::string(ASSET_PATH) + "/ui/Background.png");
}

void MainMenuState::render(sf::RenderWindow& window)
{
    m_playButton.render();
    m_quitButton.render();
    m_backgroundGraphics.render(window);
}

void MainMenuState::update(const float deltaTime)
{
}