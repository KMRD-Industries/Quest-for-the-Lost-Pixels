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
    static void setOrigin(Entity);
    static void setSpritePosition(Entity);
    static void displayDamageTaken(Entity);
    static void displayWeaponStatsTable(const sf::RenderWindow&, Entity entity);
    static void displayPlayerStatsTable(const sf::RenderWindow&, Entity entity);
};
