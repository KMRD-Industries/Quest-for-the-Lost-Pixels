#include "StateMachine.h"

#include "State.h"

void StateManager::pushState(const std::shared_ptr<State>& newState, const MenuStateMachine::StateAction action)
{
    if (action != MenuStateMachine::StateAction::PutOnTop)
    {
        m_states.push(newState);
        m_states.top()->beforeInit();
        m_states.top()->init();
    }
    else
    {
        m_statesToRenderOnTop.push_back(newState);
        m_statesToRenderOnTop.back()->beforeInit();
        m_statesToRenderOnTop.back()->init();
    }
}

void StateManager::popState()
{
    if (!m_statesToRenderOnTop.empty())
    {
        m_statesToRenderOnTop.pop_back();
        return;
    }

    if (!m_states.empty())
        m_states.pop();
    if (!m_states.empty())
    {
        m_states.top()->beforeInit();
        m_states.top()->init();
    }
}

void StateManager::update(const float deltaTime)
{
    if (!m_statesToRenderOnTop.empty())
    {
        m_statesToRenderOnTop.back()->update(deltaTime);
        return;
    }
    if (!m_states.empty())
        m_states.top()->update(deltaTime);
}

void StateManager::render(sf::RenderWindow& window)
{
    if (!m_states.empty())
        m_states.top()->render(window);
    for (auto& state : m_statesToRenderOnTop)
        state->render(window);
}

void StateManager::handleStateChange(const std::vector<MenuStateMachine::StateAction>& actionList,
                                     std::vector<std::optional<std::shared_ptr<State>>> newStateList)
{
    for (int index = 0; index < actionList.size(); ++index)
    {
        const auto& action = actionList.at(index);
        if (action == MenuStateMachine::StateAction::Pop)
            popState();


        auto callback =
            [this](const std::vector<MenuStateMachine::StateAction>& actionList,
                   const std::vector<std::optional<std::shared_ptr<State>>>& newStateList)
        {
            this->handleStateChange(actionList, newStateList);
        };
        if (newStateList[index])
        {
            newStateList[index].value()->m_stateChangeCallback = callback;
            pushState(newStateList[index].value(), action);
        }
    }
}