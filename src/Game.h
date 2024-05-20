#pragma once
#include "Dungeon.h"

class Game
{
public:
    void init();
    void draw() const;
    void update();

private:
    Dungeon m_dungeon;
};