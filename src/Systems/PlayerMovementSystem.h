#pragma once

#include "InputHandler.h"
#include "System.h"

class PlayerMovementSystem : public System
{
public:
    void init();
    void update(float);
    PlayerMovementSystem();

private:
    void handleMovement();
    void handleAttack() const;
    void handlePickUpAction();
    void handleSpecialKeys();
    InputHandler* inputHandler = nullptr;
    float m_frameTime{};
};