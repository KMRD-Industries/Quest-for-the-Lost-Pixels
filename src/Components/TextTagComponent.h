#pragma once

#include "AtlasComponents/Texture.h"
#include "Config.h"

struct TextTagComponent
{
    sf::Text text{};
    sf::Font font{};
    sf::Color color{sf::Color::Red};
    int size{config::textTagDefaultSize};
    float lifetime{config::textTagDefaultLifetime};
    float speed{config::textTagDefaultSpeed};
    float acceleration{config::textTagDefaultAcceleration};
    int fade_value{config::textTagDefaultFadeValue};
};
