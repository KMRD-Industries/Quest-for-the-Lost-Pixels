
#pragma once

#include "System.h"

class WeaponSystem : public System
{
public:
    void update() const;

private:
    static void updateWeaponAngle(Entity entity);
    static void updateStartingAngle(Entity entity);
    static void rotateForward(Entity entity);
    static void rotateBackward(Entity entity);

    static constexpr int MAX_LEFT_FACING_ANGLE = 420;
};
