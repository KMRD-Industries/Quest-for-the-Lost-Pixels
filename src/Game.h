#include "Dungeon.h"

class Game
{
public:
    void init() { m_dungeon.init(); };
    void draw() const { m_dungeon.draw(); };
    void update() { m_dungeon.update(); };

private:
    Dungeon m_dungeon;
};