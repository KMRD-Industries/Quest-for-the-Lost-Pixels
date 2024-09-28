#pragma once

#include "State.h"

class MainMenuState : public State
{
public:
    MainMenuState() : m_entities(MAX_ENTITIES - 1)
    {
    };

    void update(float deltaTime) override;
    void render() override;
    void init() override;

private:
    std::vector<Entity> m_entities{};
};