#pragma once

#include "Button.h"
#include "Dungeon.h"
#include "Graphics.h"
#include "State.h"

class MainMenuState : public State
{
public:
    MainMenuState() : m_entities(MAX_ENTITIES - 1)
    {
    };

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void init() override;

private:
    std::vector<Entity> m_entities{};
    Button m_playButton{};
    Button m_quitButton{};
    Graphics m_backgroundGraphics{};
};