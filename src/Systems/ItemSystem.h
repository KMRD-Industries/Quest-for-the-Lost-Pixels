#pragma once

#include "System.h"
#include "Types.h"
#include "WeaponComponent.h"

class ItemSystem : public System
{
public:
    void init();
    void update();
    void markClosest();
    void displayWeaponStats(Entity entity);
    void displayHelmetStats(Entity entity);
    void displayBodyArmourStats(Entity entity);
    void input(Entity player);
    void deleteItems() const;

private:
    float m_frameTime{};
    GameType::PickUpInfo closestItem{};
};
