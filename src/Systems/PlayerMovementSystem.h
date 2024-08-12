#pragma once

#include <memory>

#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void update();

private:
    void handleMovement();
    void handleAttack();
    void handleMousePositionUpdate() const;
    bool flip = false;
};
