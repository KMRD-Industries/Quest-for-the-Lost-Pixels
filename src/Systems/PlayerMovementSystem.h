#pragma once

#include <memory>

#include "InputHandler.h"
#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void init();
    void update();
    PlayerMovementSystem();

private:
    void handleMovement();
    void handleAttack() const;
    bool flip = false;
    InputHandler* inputHandler = nullptr;
};
