#include "EndGameState.h"

#include "ButtonDTO.h"
#include "GameState.h"
#include "Paths.h"
#include "TextDTO.h"

void EndGameState::init()
{
    m_menuButton.init(ButtonDTO{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                ASSET_PATH + std::string("/ui/uiFont.ttf"),
                                "Menu",
                                40.f,
                                2.f,
                                {620, 644},
                                {0, 387, 144, 42},
                                {144, 387, 144, 42},
                                {288, 387, 144, 42},
                                {0, 0},
                                {1, 1},
                                [this]() { m_returnToMenu = true; }});

    m_restartButton.init(ButtonDTO{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                   ASSET_PATH + std::string("/ui/uiFont.ttf"),
                                   "Restart",
                                   40.f,
                                   2.f,
                                   {1010, 644},
                                   {0, 387, 144, 42},
                                   {144, 387, 144, 42},
                                   {288, 387, 144, 42},
                                   {0, 0},
                                   {1, 1},
                                   [this]() { m_restartGame = true; }});

    m_quitButton.init(ButtonDTO{std::string(ASSET_PATH) + "/ui/uiHigh.png",
                                ASSET_PATH + std::string("/ui/uiFont.ttf"),
                                "Quit",
                                40.f,
                                2.f,
                                {815, 780},
                                {0, 387, 144, 42},
                                {144, 387, 144, 42},
                                {288, 387, 144, 42},
                                {0, 0},
                                {1, 1},
                                []() { std::exit(1); }});

    m_backgroundGraphics.init(std::string(ASSET_PATH) + "/ui/Pngs/SettingsBackground.png");
    m_crown.init(std::string(ASSET_PATH) + "/ui/crown.png");
    m_winText.init(
        TextDTO{ASSET_PATH + std::string("/ui/uiFont.ttf"), "You won!!", 160, 2.f, {0.f, 0.f, 0.f, 1.f}, {680, 200}});
}

void EndGameState::render(sf::RenderTexture& window)
{
    m_backgroundGraphics.render(window);
    m_crown.render(window);
    m_menuButton.render();
    m_restartButton.render();
    m_quitButton.render();
    m_winText.render();
}

void EndGameState::update(const float deltaTime)
{
    if (m_returnToMenu)
    {
        m_stateChangeCallback({MenuStateMachine::StateAction::Pop, MenuStateMachine::StateAction::Pop},
                              {std::nullopt, std::nullopt});
    }
    if (m_restartGame)
    {
        m_stateChangeCallback({MenuStateMachine::StateAction::Pop, MenuStateMachine::StateAction::Pop},
                              {std::nullopt, std::make_unique<GameState>()});
    }
}