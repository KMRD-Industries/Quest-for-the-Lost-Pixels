#pragma once

#include "System.h"

namespace sf
{
    class RenderTexture;
    class RenderWindow;
} // namespace sf

class BackgroundSystem : public System
{
public:
    void draw(sf::RenderTexture& window);
};