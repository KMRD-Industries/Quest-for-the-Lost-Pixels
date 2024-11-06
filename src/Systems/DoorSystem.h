#pragma once

#include "GameTypes.h"
#include "SFML/System/Vector2.hpp"
#include "System.h"

class DoorSystem : public System
{
public:
    DoorSystem() = default;
    void update();
    void clearDoors() const;
    [[nodiscard]] sf::Vector2f getDoorPosition(GameType::DoorEntraces entrance) const;
};