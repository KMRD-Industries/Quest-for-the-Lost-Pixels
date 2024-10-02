#pragma once

#include "Button.h"
#include "Graphics.h"
#include "State.h"

class MainMenuState : public State
{
public:
    MainMenuState()
    {
    };

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void init() override;

private:
    Button m_playButton{};
    Button m_quitButton{};
    Graphics m_backgroundGraphics{};
};