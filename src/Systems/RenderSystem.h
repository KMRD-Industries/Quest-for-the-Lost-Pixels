#pragma once

#include <set>
#include <vector>
#include "Camera.h"
#include "Config.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderTexture.hpp"
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
    void draw(sf::RenderTexture& window);
    void clearMap();

private:
    void displayPortal(const Entity entity);
    void updateCamera(Camera& camera, const sf::Vector2f& targetPos, const sf::RenderTexture& window);
    void updateQuad(const Entity entity);
    void updatePlayerSprite(const Entity entity);
    void setEquipment(const Entity entity);
    sf::Vector2f getEquippedItemOrigin(const Collision& itemPlacement);
    sf::Vector2f getEquippedItemPosition(const Collision& itemPivot, const Collision& itemPlacement,
                                         const RenderComponent& playerSprite);
    float getEquippedItemRotation(Entity entity);
    void reverseDisplay(Collision& itemPivot, Collision& itemPlacement);
    void debugBoundingBoxes(sf::RenderTexture& window);
    void drawEquipment(Entity entity);

    sf::Vector2f getPosition(const Entity entity);
    sf::Vector2f getScale(const Entity entity);
    sf::Vector2f getOrigin(const Entity entity);
    float getRotation(const Entity entity);

    void updateSprite(const Entity entity);
    void clearSpriteArray();
    void displayDamageTaken(const Entity entity);
    void displayWeaponStatsTable(const sf::RenderTexture& window, const Entity entity);
    void displayPlayerStatsTable(const sf::RenderTexture& window, const Entity entity) const;

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