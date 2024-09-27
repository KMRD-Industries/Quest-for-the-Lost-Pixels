#pragma once
#include <memory>
#include <optional>
#include <stack>

#include "State.h"

enum class StateAction
{
    Push,
    Pop
};

class StateManager
{
public:
    void pushState(std::unique_ptr<State> newState);
    void popState();
    void update(float deltaTime);
    void render();
    void handleStateChange(StateAction action, std::optional<std::unique_ptr<State>> newState);
    void handleStateChange(StateAction action);

private:
    std::stack<std::unique_ptr<State>> m_states;
};