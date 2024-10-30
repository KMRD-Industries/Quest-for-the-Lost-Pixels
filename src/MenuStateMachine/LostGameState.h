#pragma once

#include "Button.h"
#include "Graphics.h"
#include "State.h"
#include "Text.h"

class LostGameState : public State
{
public:
    LostGameState(const bool resetECS = false) : State(resetECS) {}

    void update(float deltaTime) override;
    void render(sf::RenderTexture& window) override;
    void init() override;

private:
    bool m_returnToMenu{};
    bool m_restartGame{};

    Text m_winText{};
    Button m_restartButton{};
    Button m_menuButton{};
    Button m_quitButton{};
    Graphics m_backgroundGraphics{.m_position = {520, 130}};
    Graphics m_tomb{.m_position = {830, 400}};
};