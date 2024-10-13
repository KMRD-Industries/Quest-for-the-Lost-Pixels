#pragma once
#include "Config.h"

struct EquipmentComponent
{
    std::unordered_map<GameType::slotType, Entity, config::slotTypeHash> slots{};
};
