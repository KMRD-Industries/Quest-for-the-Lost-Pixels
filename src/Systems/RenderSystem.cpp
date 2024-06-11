#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    std::vector<std::vector<sf::Sprite>> tiles(5);
    sf::Vector2<unsigned int> window_size = window.getSize();
    float max_x = 0;
    float max_y = 0;

    for (const auto& entity : m_entities)
    {
        if (auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity); layer > 0 && layer < 5)
        {
            const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            const auto& collisionComponent = gCoordinator.getComponent<ColliderComponent>(entity);

            sf::FloatRect spriteBounds = sprite.getLocalBounds();
            max_x = std::max(max_x, transformComponent.position.x);
            max_y = std::max(max_y, transformComponent.position.y);

            sprite.setOrigin(spriteBounds.width / 2.f, spriteBounds.height / 2.f);
            sprite.setScale(transformComponent.scale * config::gameScale);
            sprite.setPosition(transformComponent.position);
            sprite.setRotation(transformComponent.rotation);
            tiles[layer].push_back(sprite);
        }
    }

    for (auto& layer : tiles)
    {
        for (auto& sprite : layer)
        {
            sprite.setPosition({sprite.getPosition().x + (window_size.x - max_x) / 2,
                                sprite.getPosition().y + (window_size.y - max_y) / 2});
            window.draw(sprite);
        }
    }
}
