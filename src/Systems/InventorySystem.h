#pragma once
#include "System.h"

class InventorySystem : public System
{
public:
    void pickUpWeapon(Entity, Entity) const;
    void dropWeapon(Entity);
};
