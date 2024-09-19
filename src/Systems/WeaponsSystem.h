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
    void updateWeaponAngle(WeaponComponent&, const RenderComponent&);
    void updateStartingAngle(WeaponComponent&, const RenderComponent&);
    void rotateForward(WeaponComponent&);
    void rotateBackward(WeaponComponent&, const RenderComponent&);
    void setAngle(WeaponComponent&, const RenderComponent&);
};
