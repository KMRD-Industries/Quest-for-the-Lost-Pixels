#pragma once

#include "RenderComponent.h"
#include "System.h"
#include "WeaponComponent.h"

class WeaponSystem : public System
{
public:
    void update() const;

private:
    static void updateWeaponAngle(WeaponComponent&, const RenderComponent&);
    static void updateStartingAngle(WeaponComponent&, const RenderComponent&);
    static void rotateForward(WeaponComponent&);
    static void rotateBackward(WeaponComponent&, const RenderComponent&);
    static void setAngle(WeaponComponent&, const RenderComponent&);

    static constexpr int MAX_LEFT_FACING_ANGLE = 420;
};
