#pragma once

#include <random>
#include "Coordinator.h"
#include "SFML/System/Vector2.hpp"

extern Coordinator gCoordinator;

class EnemySystem : public System
{
public:
    EnemySystem();
    void init();
    void update(std::unordered_map<Entity, sf::Vector2<float>>&);
    void deleteEnemies() const;

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;
};
