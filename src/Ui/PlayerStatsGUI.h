#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "Types.h"

class PlayerStatsGUI
{
public:
    void displayPlayerStatsTable(const sf::RenderWindow& window, Entity entity) const;
    void displayWeaponStatsTable(const sf::RenderWindow& window, Entity entity);
    void displayEntityExplorator(const sf::RenderWindow& window);
};
