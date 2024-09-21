#pragma once
#include "Dungeon.h"

class Game
{
public:
    void init();
    void draw();
    void update(float deltaTime);
    void handleCollision();

private:
    Dungeon m_dungeon;
};