#include "Graphics.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

void Graphics::render(sf::RenderWindow& window) { window.draw(*m_sprite); }

void Graphics::init(const std::string& spritePath)
{
    sf::Texture* backgroundTexture = new sf::Texture();
    if (!backgroundTexture->loadFromFile(spritePath)) return;
    m_sprite = new sf::Sprite();
    m_sprite->setTexture(*backgroundTexture);
    m_sprite->setPosition(m_position);
    m_sprite->setRotation(m_rotation);
    m_sprite->setScale(m_scale);
}