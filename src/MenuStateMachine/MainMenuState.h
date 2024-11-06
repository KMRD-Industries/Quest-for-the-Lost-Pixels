#pragma once

#include "Button.h"
#include "Graphics.h"
#include "InputText.h"
#include "State.h"

class MainMenuState : public State
{
public:
    MainMenuState(){};

    void update(float deltaTime) override;
    void render(sf::RenderTexture& window) override;
    void init() override;

private:
    InputText m_inputIP{};
    Button m_playButton{};
    Button m_quitButton{};
    Graphics m_backgroundGraphics{};
};