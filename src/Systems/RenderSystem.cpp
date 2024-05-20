#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    std::vector<std::vector<sf::Sprite>> tiles(5);

    for (const auto& entity : m_entities)
    {
        if (auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity); layer > 0)
        {
            const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            sprite.setScale(transformComponent.scale * config::gameScale);
            sprite.setPosition(transformComponent.position);
            sprite.setRotation(transformComponent.rotation);
            tiles[layer].push_back(sprite);
        }
    }

    for (const auto& pair : tiles)
    {
        for (const auto& sprite : pair)
        {
            window.draw(sprite);
        }
    }
}
