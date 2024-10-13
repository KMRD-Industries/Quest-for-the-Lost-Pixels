#pragma once
#include "Config.h"
#include "GameTypes.h"
#include "System.h"

class InventorySystem : public System
{
public:
    void pickUpItem(GameType::PickUpInfo) const;
    void dropItem(Entity player, Entity item, GameType::slotType) const;
};
