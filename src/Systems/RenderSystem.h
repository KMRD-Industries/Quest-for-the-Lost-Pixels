#pragma once

#include "Config.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include "System.h"
#include <vector>

namespace sf
{
    class RenderWindow;
}

class RenderSystem : public System
{
public:
    RenderSystem();
    void init();
    void update();
    void displayPortal(Entity entity);
    void draw(sf::RenderWindow& window);
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

private:
    void displayEnemiesTable(const sf::RenderWindow& window);
    void debugBoundingBoxes(sf::RenderWindow& window);
    void drawEquipment(Entity);
    void setOrigin(Entity);
    void setSpritePosition(Entity);
    sf::Vector2f getPosition(Entity);
    sf::Vector2f getScale(Entity entity);
    float getRotation(Entity entity);
    sf::Vector2f getOrigin(Entity);
    void updateSprite(Entity);
    void clear(const sf::Window& window);
    void displayDamageTaken(Entity);
    void displayWeaponStatsTable(const sf::RenderWindow&, Entity entity);
    void displayPlayerStatsTable(const sf::RenderWindow&, Entity entity) const;

    std::vector<std::vector<std::pair<sf::Sprite*, bool*>>> tiles;
    sf::Sprite portalSprite;
    std::vector<Entity> players;
    sf::Vector2u windowSize{};
    sf::Vector2f newMapOffset{};
};
