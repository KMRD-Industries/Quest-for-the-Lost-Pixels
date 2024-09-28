#pragma once
#include "System.h"

class EquipWeaponSystem : public System
{
public:
    void init();
    void update();
    void equipWeapon(Entity, Entity);
};
