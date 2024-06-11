#pragma once

#include <memory>

#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void update() const;

private:
    void handleMovement() const;
    void handleAttack() const;
};
