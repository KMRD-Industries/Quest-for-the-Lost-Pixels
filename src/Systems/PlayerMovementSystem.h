#pragma once

#include <memory>

#include "InputHandler.h"
#include "System.h"

class PlayerMovementSystem : public System {
public:
    void init();

    void update(float);

    void handleSpecialKeys();

    PlayerMovementSystem();

private:
    void handleMovement();

    void handleAttack() const;

    void handlePickUpAction();

    InputHandler *inputHandler = nullptr;
    float m_frameTime{};
};
