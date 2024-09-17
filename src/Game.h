#pragma once
#include "Dungeon.h"

class Game
{
public:
    static void init();
    static void draw();
    static void update();
    static void handleCollision();

private:
    static Dungeon m_dungeon;
};