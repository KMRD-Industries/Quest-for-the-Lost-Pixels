#pragma once
#include "System.h"

class InventorySystem : public System
{
public:
    void init();
    void update();
    void pickUpWeapon(Entity, Entity);
    void dropWeapon(Entity);
};
