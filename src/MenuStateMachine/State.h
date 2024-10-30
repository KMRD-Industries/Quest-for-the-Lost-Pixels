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
using StateChangeCallback = std::function<void(const std::vector<MenuStateMachine::StateAction>& actionList,
                                               std::vector<std::optional<std::shared_ptr<State>>> newStateList)>;

class State
{
public:
    State(const bool resetECS = true) : m_resetECS{resetECS} {}

    virtual ~State() = default;
    void beforeInit();
    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    StateChangeCallback m_stateChangeCallback;
    bool m_resetECS{};
};