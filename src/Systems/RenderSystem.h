#pragma once

#include <set>
#include <vector>
#include "Camera.h"
#include "Config.h"
#include "RenderComponent.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include "System.h"
#include "TransformComponent.h"

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
    void clearMap();

private:
    void displayPortal(Entity entity);
    void renderMap(sf::RenderWindow& window, const std::vector<sf::Sprite*>& tiles, const Camera& camera);
    void renderMap(sf::RenderWindow& window, const std::unordered_map<sf::Texture*, sf::VertexArray>& tiles,
                   const Camera& camera);
    void updateCamera(Camera& camera, sf::Vector2f targetPos, const sf::RenderWindow& window);
    void updateQuad(Entity entity);
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
    void clearSpriteArray();
    void displayDamageTaken(Entity entity);
    void displayWeaponStatsTable(const sf::RenderWindow& window, Entity entity);
    void displayPlayerStatsTable(const sf::RenderWindow& window, Entity entity) const;

    std::vector<std::vector<sf::Sprite*>> m_vecSpriteArray;
    std::vector<unsigned long> m_entityToVertexArrayIndex;
    std::vector<std::unordered_map<sf::Texture*, sf::VertexArray>> m_layeredTextureVertexArrays;
    std::set<Entity> players;
    std::set<Entity> specialObjects;
    sf::Vector2u windowSize{};
    sf::Vector2f newMapOffset{};
    sf::Sprite portalSprite;
    Camera camera{};
};