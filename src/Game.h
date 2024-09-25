#pragma once
#include "Dungeon.h"
#include "StateMachine.h"

class Game
{
public:
    void init();
    void draw();
    void update(float deltaTime);
    void handleCollision();

private:
    Dungeon m_dungeon;
    StateManager m_stateManager;
};