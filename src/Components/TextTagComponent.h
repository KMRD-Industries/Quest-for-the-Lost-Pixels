#pragma once

#include "AtlasComponents/Texture.h"

struct TextTagComponent
{
    sf::Text text{};
    sf::Font font{};
    sf::Color color{sf::Color::Red};
    int size{20};
    float lifetime{60};
    float speed{3};
    float acceleration{10};
    int fade_value{20};
};
