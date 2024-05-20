#pragma once
#include "Dungeon.h"

class Game
{
public:
    void init();
    void draw() const;
    void update();
    void handleCollision();

private:
    Dungeon m_dungeon;
};