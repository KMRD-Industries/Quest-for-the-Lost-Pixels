#pragma once
#include "StateMachine.h"

class Game
{
public:
    void init();
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);

private:
    StateManager m_stateManager;
};