#pragma once

#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void update();

private:
    void handleMovement();
    void handleAttack() const;
    void handleMousePositionUpdate() const;
    bool flip = false;
};