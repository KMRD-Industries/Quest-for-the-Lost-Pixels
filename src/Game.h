#pragma once
#include "Dungeon.h"
#include "SFML/Graphics/RenderWindow.hpp"


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
