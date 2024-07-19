#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <utility>

struct Texture : sf::Texture
{
    explicit Texture(std::shared_ptr<sf::Texture> texture = nullptr, const sf::IntRect rect = sf::IntRect()) :
        m_texture{std::move(texture)}, m_rect{rect}
    {
    }

    std::shared_ptr<sf::Texture> m_texture;
    sf::IntRect m_rect{};
};
