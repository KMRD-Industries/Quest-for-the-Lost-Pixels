#include "GameState.h"

#include "CollisionSystem.h"
#include "Coordinator.h"
#include "FightSystem.h"
#include "ObjectCreatorSystem.h"

extern Coordinator gCoordinator;

void GameState::init()
{
    m_dungeon.m_stateChangeCallback = m_stateChangeCallback;
    m_dungeon.init();
}

void GameState::handleCollision(const float deltaTime)
{
    const auto collisionSystem = gCoordinator.getRegisterSystem<CollisionSystem>();
    constexpr auto timeStep = 1.f / 60.f;
    collisionSystem->update();
    collisionSystem->updateSimulation(timeStep, 8, 3);
}

void GameState::render(sf::RenderWindow &window) { m_dungeon.render(window); }

void GameState::update(const float deltaTime)
{
    gCoordinator.getRegisterSystem<ObjectCreatorSystem>()->update();
    gCoordinator.getRegisterSystem<FightSystem>()->update();
    handleCollision(deltaTime);
    m_dungeon.update(deltaTime);
    gCoordinator.getRegisterSystem<CollisionSystem>()->deleteMarkedBodies();
}
