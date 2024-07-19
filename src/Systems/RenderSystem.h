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
    float m_mapRenderOffsetY = 0.f;
    float m_mapRenderOffsetX = 0.f;
};
