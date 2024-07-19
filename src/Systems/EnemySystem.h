#pragma once

#include "Coordinator.h"
extern Coordinator gCoordinator;

class EnemySystem : public System
{
public:
    void update() const;
    void deleteEnemies() const;
};
