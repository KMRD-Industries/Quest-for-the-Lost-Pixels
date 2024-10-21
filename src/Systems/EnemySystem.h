#pragma once

#include <random>
#include "Coordinator.h"

extern Coordinator gCoordinator;

class EnemySystem : public System
{
public:
    EnemySystem();
    void init();
    void performFixedUpdate();
    void update(const float&);
    void deleteEnemies() const;

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
    float m_frameTime{};
};