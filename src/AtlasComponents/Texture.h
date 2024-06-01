#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <utility>

struct Texture : sf::Texture
{
    explicit Texture(std::shared_ptr<sf::Texture> texture = nullptr) : m_texture{std::move(texture)} {}

    std::shared_ptr<sf::Texture> m_texture;
};
