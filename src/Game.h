#pragma once
#include <RenderSystem.h>
#include <TextureSystem.h>

#include "CollisionSystem.h"
#include "Coordinator.h"
#include "Game.h"
#include "StateMachine.h"

class Game
{
public:
    void init();
    void draw(sf::RenderTexture& window);
    void update(float deltaTime);

private:
    StateManager m_stateManager;
    CollisionSystem* m_collisionSystem{nullptr};
};