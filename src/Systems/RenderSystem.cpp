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
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        if (renderComponent.layer > 0)
        {
            const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);

            renderComponent.sprite.setScale(transformComponent.scale * config::gameScale);
            renderComponent.sprite.setPosition(transformComponent.position);
            renderComponent.sprite.setRotation(transformComponent.rotation);
            tiles[renderComponent.layer].push_back(renderComponent.sprite);
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
