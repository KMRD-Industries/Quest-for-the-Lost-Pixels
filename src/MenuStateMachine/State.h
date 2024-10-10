#pragma once
#include <functional>
#include <memory>
#include <optional>

#include "GameTypes.h"

namespace sf
{
    class RenderWindow;
}

class State;
using StateChangeCallback = std::function<void(MenuStateMachine::StateAction action,
                                               std::optional<std::unique_ptr<State>> newState)>;

class State
{
public:
    virtual ~State() = default;
    void beforeInit();
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    StateChangeCallback m_stateChangeCallback;
};