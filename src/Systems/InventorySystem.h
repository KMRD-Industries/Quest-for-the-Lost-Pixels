#pragma once
#include "System.h"

class InventorySystem : public System
{
public:
    static void pickUpWeapon(Entity, Entity);
    void dropWeapon(Entity, Entity);
};
