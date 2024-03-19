#pragma once

#include "System.h"

namespace sf {
class RenderWindow;
}

class RenderSystem : public System
{
public:
    void draw(sf::RenderWindow& window) const;
};
