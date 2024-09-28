#pragma once

#include "RenderComponent.h"
#include "System.h"
#include "WeaponComponent.h"

class WeaponSystem : public System
{
public:
    void init();
    void update();

private:
    void updateWeaponAngle(Entity);
    void updateStartingAngle(Entity);
    void setAngle(Entity);
    void rotateWeapon(Entity, bool);
};
