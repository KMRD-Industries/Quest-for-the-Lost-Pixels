#pragma once
#include <memory>
#include <optional>
#include <stack>

#include "State.h"

class StateManager
{
public:
    void handleStateChange(MenuStateMachine::StateAction action, std::optional<std::unique_ptr<State>> newState);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    void pushState(std::unique_ptr<State> newState);
    void popState();
    std::stack<std::unique_ptr<State>> m_states;
};