#pragma once
#include "Config.h"
#include "System.h"

class InventorySystem : public System
{
public:
    void pickUpItem(Entity player, Entity item, config::slotType) const;
    void dropItem(Entity player, Entity item, config::slotType) const;
};
