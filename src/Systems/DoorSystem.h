#pragma once

#include "GameTypes.h"
#include "SFML/System/Vector2.hpp"
#include "System.h"

class DoorSystem : public System
{
public:
    void init();
    void update();
    void clearDoors() const;
    [[nodiscard]] sf::Vector2f getDoorPosition(GameType::DoorEntraces entrance) const;
};