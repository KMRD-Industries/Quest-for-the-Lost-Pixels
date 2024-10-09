#pragma once
#include "Config.h"

struct InventoryComponent {
    std::unordered_map<config::slotType, Entity, config::slotTypeHash> slots{};
};
