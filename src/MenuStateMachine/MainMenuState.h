#pragma once

#include <imgui.h>
#include "State.h"

class MainMenuState : public State
{
public:
    void update(float deltaTime) override;
    void render() override;
    void init() override;
    ImVec2 m_uv0;
    ImVec2 m_uv1;
};