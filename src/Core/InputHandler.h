#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Helpers.h"

#include "SFML/Window/Keyboard.hpp"

enum class InputType
{
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown
};

struct KeyHash
{
    std::size_t operator()(const sf::Keyboard::Key& key) const { return std::hash<int>()(static_cast<int>(key)); }
};

struct InputTypeHash
{
    std::size_t operator()(const InputType& input) const { return std::hash<int>()(static_cast<int>(input)); }
};

class InputHandler
{
    std::unordered_set<InputType, InputTypeHash> m_keysHeld{};
    std::unordered_set<InputType, InputTypeHash> m_keysPressed{};
    std::unordered_map<sf::Keyboard::Key, InputType, KeyHash> m_keyToMapInput{
        {sf::Keyboard::Key::W, InputType::MoveUp},
        {sf::Keyboard::Key::A, InputType::MoveLeft},
        {sf::Keyboard::Key::S, InputType::MoveDown},
        {sf::Keyboard::Key::D, InputType::MoveRight}};
    inline static InputHandler* m_instance{};
    InputHandler() = default;

public:
    static auto getInstance() -> InputHandler*
    {
        if (m_instance == nullptr)
        {
            m_instance = new InputHandler();
        }
        return m_instance;
    };
    [[nodiscard]] auto isHeld(InputType input) const -> bool;
    [[nodiscard]] bool isPressed(InputType input) const;
    void handleKeyboardInput(sf::Keyboard::Key key, bool isPressed);
    void clearPressedInputs();
    void update();

protected:
    void updateKey(InputType input, bool isPressed);
};