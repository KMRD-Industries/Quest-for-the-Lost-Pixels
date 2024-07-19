#pragma once

#include "System.h"

namespace sf
{
    class RenderWindow;
}

class RenderSystem : public System
{
public:
    void draw(sf::RenderWindow& window);

private:
    void debugBoundingBoxes(sf::RenderWindow& window) const;
    float mapRenderOffsetY = 0.f;
    float mapRenderOffsetX = 0.f;
};
