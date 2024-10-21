#pragma once

#include "System.h"
#include "WeaponComponent.h"

class WeaponSystem : public System
{
public:
    void update(const float &deltaTime);
    void performFixedUpdate();

private:
    void updateWeaponAngle(Entity);
    void updateStartingAngle(Entity);
    void setAngle(Entity);
    void rotateWeapon(Entity, bool);
    float m_frameTime{};
};