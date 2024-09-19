#pragma once

#include "Coordinator.h"
extern Coordinator gCoordinator;

class EnemySystem : public System
{
public:
    void init();
    void update();
    void deleteEnemies() const;
};
