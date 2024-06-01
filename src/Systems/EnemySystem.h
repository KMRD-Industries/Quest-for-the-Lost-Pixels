#pragma once

#include "Coordinator.h"
extern Coordinator gCoordinator;

class EnemySystem : public System
{
private:
public:
    Entity getFirstUnused();
    void update();
};
