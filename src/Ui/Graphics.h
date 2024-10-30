#pragma once
#include <string>

#include "SFML/System/Vector2.hpp"


namespace sf
{
    class RenderWindow;
    class Sprite;
} // namespace sf

struct Graphics
{
    void render(sf::RenderWindow& window);
    void init(const std::string& spritePath);

    sf::Sprite* m_sprite{};
    sf::Vector2f m_position{0.f, 0.f};
    sf::Vector2f m_scale{1.f, 1.f};
    float m_rotation{};
};