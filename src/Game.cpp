#include "Game.h"
void Game::init() { m_dungeon.init(); };
void Game::draw() const { m_dungeon.draw(); };
void Game::update() { m_dungeon.update(); };