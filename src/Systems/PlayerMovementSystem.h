#pragma once

#include <memory>

#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void update();

private:
    void handleMovement();
    void handleAttack() const;
    bool flip = false;
};
