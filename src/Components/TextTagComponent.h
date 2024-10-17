#pragma once

#include "PublicConfigMenager.h"
#include "AtlasComponents/Texture.h"

extern PublicConfigSingleton configSingleton;

struct TextTagComponent
{
    sf::Text text{};
    sf::Font font{};
    sf::Color color{sf::Color::Red};
    int size{configSingleton.GetConfig().textTagDefaultSize};
    float lifetime{configSingleton.GetConfig().textTagDefaultLifetime};
    float speed{configSingleton.GetConfig().textTagDefaultSpeed};
    float acceleration{configSingleton.GetConfig().textTagDefaultAcceleration};
    int fade_value{configSingleton.GetConfig().textTagDefaultFadeValue};
};