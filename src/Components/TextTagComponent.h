#pragma once

#include <utility>
#include <vector>
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"
#include "CircularIterator.h"

struct TextTagComponent
{
    sf::Text text{};
    sf::Font font{};
    sf::Color color{sf::Color::Red};
    int size{20};
    float lifetime{20};
    float speed{10};
    float acceleration{20};
    int fade_value{20};
};
