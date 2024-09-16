#pragma once

#include "RenderComponent.h"
#include "System.h"
#include "WeaponComponent.h"

class WeaponSystem : public System
{
public:
    void update();

private:
    void updateWeaponAngle(WeaponComponent&, const RenderComponent&);
    void updateStartingAngle(WeaponComponent&, const RenderComponent&);
    void rotateForward(WeaponComponent&);
    void rotateBackward(WeaponComponent&, const RenderComponent&);
    void setAngle(WeaponComponent&, const RenderComponent&);

    static constexpr int MAX_LEFT_FACING_ANGLE = 420;
};
