#pragma once
#include "System.h"

class EquipWeaponSystem : public System
{
public:
    void init() const;
    void update() const;
    void equipWeapon(Entity, Entity) const;
    void equipHelmet(Entity player, Entity helmet) const;
};
