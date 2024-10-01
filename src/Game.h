#pragma once
#include "Dungeon.h"
#include "StateMachine.h"

class Game
{
public:
    void init();
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);
    void handleCollision();

private:
    StateManager m_stateManager;
};