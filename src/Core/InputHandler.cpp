#include "InputHandler.h"

bool InputHandler::isHeld(const InputType input) const { return m_keysHeld.contains(input); }

bool InputHandler::isPressed(const InputType input) const { return m_keysPressed.contains(input); }

sf::Vector2f InputHandler::getMousePosition() const { return m_mousePosition; }

void InputHandler::handleKeyboardInput(const InputKey& key, const bool& isPressed)
{
    const auto [fst, snd]{m_keyToMapInput.equal_range(key)};
    if (fst == m_keyToMapInput.end())
    {
        return;
    }
    for (auto it{fst}; it != snd; ++it)
    {
        updateKey(it->second, isPressed);
    }
}

void InputHandler::updateMousePosition(const sf::Vector2f& newMousePosition)
{
    this->m_mousePosition = newMousePosition;
}

void InputHandler::clearPressedInputs() { m_keysPressed.clear(); }

void InputHandler::update() { clearPressedInputs(); }

void InputHandler::updateKey(const InputType input, const bool isPressed)
{
    if (isPressed && !isHeld(input))
    {
        m_keysPressed.insert(input);
    }
    else if (!isPressed)
    {
        m_keysPressed.erase(input);
    }

    if (isPressed)
    {
        m_keysHeld.insert(input);
    }
    else
    {
        m_keysHeld.erase(input);
    }
}
