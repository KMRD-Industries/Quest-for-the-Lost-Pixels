
#pragma once

#include "System.h"

class WeaponSystem : public System
{
public:
    void update();

private:
    void updateWeaponAngle(Entity entity);
    void updateStartingAngle(Entity entity);
    void rotateForward(Entity entity);
    void rotateBackward(Entity entity);

    static constexpr int MAX_LEFT_FACING_ANGLE = 420;
};
