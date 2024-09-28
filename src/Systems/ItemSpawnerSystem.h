#pragma once

#include "Coordinator.h"
#include "GameTypes.h"
#include "System.h"

class ItemSpawnerSystem : public System
{
public:
    ItemSpawnerSystem();
    void init();
    void updateAnimation(float deltaTime);
    void deleteItems();

private:
    void handlePotionCollision(Entity potion, const GameType::CollisionData& collisionData, Items::Behaviours behaviour,
                               float value);
};