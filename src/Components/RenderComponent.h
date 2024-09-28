#pragma once

#include "SFML/Graphics/Sprite.hpp"

struct RenderComponent
{
    sf::Sprite sprite{};
    int layer{};
};
