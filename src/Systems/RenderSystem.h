#pragma once

#include "SFML/System/Vector2.hpp"
#include "System.h"

namespace sf
{
    class RenderWindow;
}

class RenderSystem : public System
{
public:
    void draw(sf::RenderWindow& window);
    sf::Vector2f mapOffset = {};

private:
    void debugBoundingBoxes(sf::RenderWindow& window) const;
    void drawEquipment(Entity) const;
    void setOrigin(Entity) const;
    void setPosition(Entity) const;
    void displayDamageTaken(Entity) const;
};
