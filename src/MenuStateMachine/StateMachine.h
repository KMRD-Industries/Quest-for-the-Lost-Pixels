#pragma once
#include <memory>
#include <optional>
#include <stack>

#include "State.h"

class StateManager
{
public:
    void handleStateChange(const std::vector<MenuStateMachine::StateAction>& actionList,
                           std::vector<std::optional<std::shared_ptr<State>>> newStateList);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    void pushState(const std::shared_ptr<State>& newState, MenuStateMachine::StateAction action);
    void popState();
    std::stack<std::shared_ptr<State>> m_states;
    std::deque<std::shared_ptr<State>> m_statesToRenderOnTop;
};