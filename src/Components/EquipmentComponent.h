#pragma once
#include "Config.h"

struct EquipmentComponent
{
    std::unordered_map<config::slotType, Entity, config::slotTypeHash> slots{};
};
