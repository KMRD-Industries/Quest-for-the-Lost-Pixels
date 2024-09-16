#pragma once
#include "System.h"

class InventorySystem : public System
{
public:
    void pickUpWeapon(Entity, Entity);
    void dropWeapon(Entity, Entity);
};
