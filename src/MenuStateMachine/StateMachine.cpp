#include "StateMachine.h"

#include "State.h"

void StateManager::pushState(std::unique_ptr<State> newState)
{
    m_states.push(std::move(newState));
    m_states.top()->beforeInit();
    m_states.top()->init();
}

void StateManager::popState()
{
    if (!m_states.empty()) m_states.pop();
    if (!m_states.empty())
    {
        m_states.top()->beforeInit();
        m_states.top()->init();
    }
}

void StateManager::update(const float deltaTime)
{
    if (!m_states.empty()) m_states.top()->update(deltaTime);
}

void StateManager::render(sf::RenderWindow& window)
{
    if (!m_states.empty()) m_states.top()->render(window);
}

void StateManager::handleStateChange(const MenuStateMachine::StateAction action,
                                     std::optional<std::unique_ptr<State>> newState)
{
    if (action == MenuStateMachine::StateAction::Pop) popState();

    if (newState)
    {
        auto callback =
            [this](const MenuStateMachine::StateAction action, std::optional<std::unique_ptr<State>> newState)
        { this->handleStateChange(action, std::move(newState)); };

        newState.value()->m_stateChangeCallback = callback;
        pushState(std::move(newState.value()));
    }
}