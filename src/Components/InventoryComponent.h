#pragma once
#include "Config.h"

struct InventoryComponent
{
    std::unordered_map<GameType::slotType, Entity, GameType::slotTypeHash> slots{};
};
