#pragma once

#include "System.h"

namespace sf
{
    class RenderWindow;
}

class BackgroundSystem : public System
{
public:
    void draw(sf::RenderWindow& window);
};