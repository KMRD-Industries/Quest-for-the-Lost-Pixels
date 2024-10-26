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
    class RenderTexture;
    class RenderWindow;
} // namespace sf

class RenderSystem : public System
{
public:
    RenderSystem();
    void init();
    void update();
    void displayPortal(Entity entity);
    void draw(sf::RenderTexture& window);
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
    void displayEnemiesTable(const sf::RenderTexture& window);
    void debugBoundingBoxes(sf::RenderTexture& window);
    void drawEquipment(Entity);
    void setOrigin(Entity);
    void setSpritePosition(Entity);
    sf::Vector2f getPosition(Entity);
    sf::Vector2f getScale(Entity entity);
    float getRotation(Entity entity);
    sf::Vector2f getOrigin(Entity);
    void updateSprite(Entity);
    void clear(const sf::RenderTexture& window);
    void displayDamageTaken(Entity);
    void displayWeaponStatsTable(const sf::RenderTexture&, Entity entity);
    void displayPlayerStatsTable(const sf::RenderTexture&, Entity entity) const;

    std::vector<std::vector<std::pair<sf::Sprite*, bool*>>> tiles;
    sf::Sprite portalSprite;
    std::vector<Entity> players;
    sf::Vector2u windowSize{};
    sf::Vector2f newMapOffset{};
};