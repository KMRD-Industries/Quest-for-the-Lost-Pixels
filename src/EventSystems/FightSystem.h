#pragma once
#include "Coordinator.h"
#include "GameTypes.h"

extern Coordinator gCoordinator;

class FightSystem : public System
{
public:
    FightSystem();
    void init() const;
    void update();
    void clear();

private:
    void handleMeleeAttack(Entity) const;
    void handleCollision(Entity bullet, const GameType::CollisionData& collisionData) const;
    float calculateAngle(const sf::Vector2f& pivotPoint, const sf::Vector2f& targetPoint) const;
    void handleWandAttack(Entity) const;
    void handleBowAttack(Entity);
};