#pragma once
#include "Dungeon.h"

class Game
{
public:
    void init();
    void draw();
    void update();
    void handleCollision();

private:
    Dungeon m_dungeon;
};
