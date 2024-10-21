#pragma once

#include "System.h"
#include "Types.h"
#include "WeaponComponent.h"

class ItemSystem : public System
{
public:
    void init();
    void update(const float& deltaTime);
    void markClosest();
    void displayWeaponStats(Entity entity);
    void displayHelmetStats(Entity entity);
    void displayBodyArmourStats(Entity entity);
    void input(Entity player);
    void deleteItems() const;

private:
    float m_frameTime{};
    double minDistance = std::numeric_limits<int>::max();
    GameType::PickUpInfo closestItem{};
    void performFixedUpdate();
};
