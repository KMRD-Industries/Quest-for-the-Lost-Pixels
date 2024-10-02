#pragma once

#include "Dungeon.h"
#include "State.h"

class GameState : public State
{
public:
    GameState()
    {
    };

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void init() override;

private:
    Dungeon m_dungeon{};
    void handleCollision(float deltaTime);
};