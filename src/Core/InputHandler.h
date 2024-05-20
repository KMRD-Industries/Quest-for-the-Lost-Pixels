#pragma once
#include <unordered_map>
#include <unordered_set>

#include "SFML/Window/Keyboard.hpp"

enum class InputType
{
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown
};

class InputHandler
{
    std::unordered_set<InputType> m_keysHeld{};
    std::unordered_set<InputType> m_keysPressed{};
    std::unordered_map<sf::Keyboard::Key, InputType> m_keyToMapInput{{sf::Keyboard::Key::W, InputType::MoveUp},
                                                                     {sf::Keyboard::Key::A, InputType::MoveLeft},
                                                                     {sf::Keyboard::Key::S, InputType::MoveDown},
                                                                     {sf::Keyboard::Key::D, InputType::MoveRight}};
    inline static InputHandler* m_instance{};
    InputHandler() = default;

public:
    static InputHandler* getInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new InputHandler();
        }
        return m_instance;
    };
    [[nodiscard]] bool isHeld(InputType input) const;
    [[nodiscard]] bool isPressed(InputType input) const;
    void handleKeyboardInput(sf::Keyboard::Key key, bool isPressed);
    void clearPressedInputs();
    void update();

protected:
    void updateKey(InputType input, bool isPressed);
};
