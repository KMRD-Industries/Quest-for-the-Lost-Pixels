#pragma once

#include "SFML/Graphics/Shape.hpp"
#include "SFML/Graphics/Sprite.hpp"

struct RenderComponent
{
    sf::Sprite sprite;
    int layer = 0;
};
