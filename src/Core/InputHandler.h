#pragma once

#include <unordered_map>
#include <unordered_set>
#include <variant>
#include "Helpers.h"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

enum class InputType
{
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Attack,
    PickUpItem
};

class InputHandler
{
    using InputKey = std::variant<sf::Keyboard::Key, sf::Mouse::Button>;

    struct InputKeyHash
    {
        std::size_t operator()(const InputKey& key) const
        {
            return std::visit(
                []<typename T0>(T0&& k) -> std::size_t
                {
                    return std::hash<std::underlying_type_t<std::decay_t<T0>>>{}(
                        static_cast<std::underlying_type_t<std::decay_t<T0>>>(k));
                },
                key);
        }
    };

    std::unordered_set<InputType> m_keysHeld{};
    std::unordered_set<InputType> m_keysPressed{};

    std::unordered_map<InputKey, InputType, InputKeyHash> m_keyToMapInput{
        {sf::Keyboard::Key::W, InputType::MoveUp},     {sf::Keyboard::Key::A, InputType::MoveLeft},
        {sf::Keyboard::Key::S, InputType::MoveDown},   {sf::Keyboard::Key::D, InputType::MoveRight},
        {sf::Keyboard::Key::Space, InputType::Attack}, {sf::Keyboard::Key::E, InputType::PickUpItem},
        {sf::Mouse::Left, InputType::Attack} // Handling for the right mouse button
    };

    inline static InputHandler* m_instance{};
    sf::Vector2f m_mousePosition{};
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
    sf::Vector2f getMousePosition() const;
    void handleKeyboardInput(const InputKey&, const bool&);
    void updateMousePosition(const sf::Vector2f&);
    void clearPressedInputs();
    void update();

protected:
    void updateKey(InputType input, bool isPressed);
};
