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
private:
    void handleMeleAttack(Entity);
    void handleCollision(Entity bullet, const GameType::CollisionData& collisionData) const;
    void handleWandAttack(Entity);
    void handleBowAttack(Entity);
    void clear();

};
