#pragma once

#include <vector>
#include "Config.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include "System.h"

namespace sf
{
    class RenderWindow;
}

class RenderSystem : public System
{
public:
    RenderSystem();
    void update();
    void draw(sf::RenderWindow& window);

private:
    void displayPortal(Entity entity);
    void updatePlayerSprite(Entity entity);
    void setHelmet() const;
    void setBodyArmour();
    void setEquipment(Entity entity);
    sf::Vector2f getEquippedItemOrigin(Collision& itemPlacement);
    sf::Vector2f getEquippedItemPosition(const Collision& itemPivot, const Collision& itemPlacement,
                                         const RenderComponent& playerSprite);
    float getEquippedItemRotation(Entity entity);
    void reverseDisplay(Collision& itemPivot, Collision& itemPlacement);
    void setWeapon();
    void setWeaponOrigin();
    void displayEnemiesTable(const sf::RenderWindow& window);
    void debugBoundingBoxes(sf::RenderWindow& window);
    void drawEquipment(Entity entity);
    void setOrigin(Entity entity);
    void setSpritePosition(Entity entity);
    sf::Vector2f getPosition(Entity entity);
    sf::Vector2f getScale(Entity entity);
    float getRotation(Entity entity);
    sf::Vector2f getOrigin(Entity entity);
    void updateSprite(Entity entity);
    void clear(const sf::Window& window);
    void displayDamageTaken(Entity entity);
    void displayWeaponStatsTable(const sf::RenderWindow& window, Entity entity);
    void displayPlayerStatsTable(const sf::RenderWindow& window, Entity entity) const;

    std::vector<std::vector<std::pair<sf::Sprite*, bool*>>> tiles;
    sf::Sprite portalSprite;
    std::vector<Entity> players;
    sf::Vector2u windowSize{};
    sf::Vector2f newMapOffset{};
};
