#pragma once

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

#include <set>

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
    void renderMap(sf::RenderWindow& window, const std::vector<sf::Sprite*>& tiles, const Camera& camera);
    void renderMap(sf::RenderWindow& window, const std::unordered_map<sf::Texture*, sf::VertexArray>& tiles,
                   const Camera& camera);
    void updateCamera(Camera& camera, sf::Vector2f targetPos, const sf::RenderWindow& window);
    void clearMap();
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
    void clearSpriteArray();
    void displayDamageTaken(Entity);
    void displayWeaponStatsTable(const sf::RenderWindow&, Entity entity);
    void displayPlayerStatsTable(const sf::RenderWindow&, Entity entity) const;

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