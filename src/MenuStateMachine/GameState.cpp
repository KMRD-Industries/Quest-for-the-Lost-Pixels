#include "GameState.h"
#include <RenderSystem.h>
#include "BackgroundSystem.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "FightSystem.h"
#include "ObjectCreatorSystem.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

void GameState::init()
{
    m_dungeon.m_stateChangeCallback = m_stateChangeCallback;
    m_collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>().get();
    m_oneFrameTime = configSingleton.GetConfig().oneFrameTime;
    m_dungeon.init();
}

void GameState::handleCollision(const float deltaTime) { m_collisionSystem->update(deltaTime, m_oneFrameTime, 8, 3); }

void GameState::render(sf::RenderWindow &window)
{
    m_dungeon.render(window);
    gCoordinator.getRegisterSystem<RenderSystem>()->draw(window);
    gCoordinator.getRegisterSystem<BackgroundSystem>()->draw(window);
    gCoordinator.getRegisterSystem<TextTagSystem>()->render(window);
}

void GameState::update(const float deltaTime)
{
    gCoordinator.getRegisterSystem<ObjectCreatorSystem>()->update();
    gCoordinator.getRegisterSystem<FightSystem>()->update();
    handleCollision(deltaTime);
    m_dungeon.update(deltaTime);
    gCoordinator.getRegisterSystem<CollisionSystem>()->deleteMarkedBodies();
}
