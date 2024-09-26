#pragma once
#include "Coordinator.h"
#include "GameTypes.h"

extern Coordinator gCoordinator;

class FightSystem : public System
{
public:
    FightSystem();
    void init();
    void update();
    void clear();

private:
    void handleMeleAttack(Entity);
    void handleCollision(Entity bullet, const GameType::CollisionData& collisionData) const;
    float calculateAngle(const sf::Vector2f& pivotPoint, const sf::Vector2f& targetPoint);
    void handleWandAttack(Entity);
    void handleBowAttack(Entity);
};
