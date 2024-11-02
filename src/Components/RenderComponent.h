#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/VertexArray.hpp"

struct RenderComponent
{
    int layer{};
    sf::Texture* texture{nullptr};
    sf::Sprite sprite{};
    sf::VertexArray vertexArray{sf::PrimitiveType::Quads};
    sf::Color color{sf::Color::White};
    bool staticMapTile{false};
    float rotation{0.0f};
};
