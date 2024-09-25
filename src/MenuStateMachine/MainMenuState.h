#pragma once

#include "State.h"

class MainMenuState : public State
{
public:
    void update(float deltaTime) override;
    void render() override;
    void init() override;
};