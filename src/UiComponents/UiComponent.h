#pragma once

#include "SFML/Graphics/Sprite.hpp"

struct UiComponent
{
    sf::Sprite* sprite;
    int layer{};
};