#pragma once

#include "RenderComponent.h"
#include "System.h"
#include "WeaponComponent.h"

class WeaponSystem : public System
{
public:
    void update() const;

private:
    static void updateWeaponAngle(Entity);
    static void updateStartingAngle(Entity);
    static void rotateForward(Entity);
    static void rotateBackward(Entity);
    static void setAngle(Entity);

    static constexpr int MAX_LEFT_FACING_ANGLE = 420;
};
