#pragma once

#include "Config.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "System.h"

#include <vector>

namespace sf
{
    class RenderWindow;
}

class RenderSystem : public System
{
public:
    void displayPortal(Entity entity);
    void draw(sf::RenderWindow& window);
    sf::Vector2f m_mapOffset = {};

private:
    void debugBoundingBoxes(sf::RenderWindow& window);
    void drawEquipment(Entity);
    void setOrigin(Entity);
    void setSpritePosition(Entity);
    void displayDamageTaken(Entity);
    void displayWeaponStatsTable(const sf::RenderWindow&, Entity entity);
    void displayPlayerStatsTable(const sf::RenderWindow&, Entity entity);

    std::vector<std::vector<sf::Sprite*>> tiles;
};
