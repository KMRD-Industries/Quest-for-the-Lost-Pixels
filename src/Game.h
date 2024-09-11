#pragma once
#include "Dungeon.h"


class Game
{
public:
    static void init();
    static void draw();
    static void update();
    static void handleCollision();
    static std::string getBackground();

private:
    static Dungeon m_dungeon;
};
