#pragma once
#include "Dungeon.h"

class Game
{
public:
    void init();
    void draw();
    void update();
    void handleCollision();
    std::string getBackground();

private:
    Dungeon m_dungeon;
};
