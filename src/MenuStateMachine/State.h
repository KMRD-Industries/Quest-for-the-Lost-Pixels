#pragma once
#include <functional>
#include <memory>
#include <optional>

#include "GameTypes.h"

class State;
using StateChangeCallback = std::function<void(MenuStateMachine::StateAction action,
                                               std::optional<std::unique_ptr<State>> newState)>;

class State
{
public:
    virtual ~State() = default;
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    StateChangeCallback m_stateChangeCallback;
};