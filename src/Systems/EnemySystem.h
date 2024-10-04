#pragma once

#include <random>
#include <stdint.h>
#include "Coordinator.h"

extern Coordinator gCoordinator;

class EnemySystem : public System
{
public:
    EnemySystem();
    void init();
    void update();
    void deleteEnemies() const;

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
};
