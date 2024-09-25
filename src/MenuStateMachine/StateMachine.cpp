#include "StateMachine.h"

#include "State.h"

void StateManager::pushState(std::unique_ptr<State> newState)
{
    m_states.push(std::move(newState));
    m_states.top()->init();
}

void StateManager::popState()
{
    if (!m_states.empty())
        m_states.pop();
}

void StateManager::update(const float deltaTime)
{
    if (!m_states.empty())
        m_states.top()->update(deltaTime);
}

void StateManager::render()
{
    if (!m_states.empty())
        m_states.top()->render();
}

void StateManager::handleStateChange(const StateAction action, std::unique_ptr<State> newState)
{
    if (action == StateAction::Pop)
        popState();
    pushState(std::move(newState));
}

void StateManager::handleStateChange(const StateAction action)
{
    if (action == StateAction::Pop)
        popState();
}