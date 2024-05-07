#include "RenderSystem.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    for (const auto& entity : m_entities)
    {
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        renderComponent.sprite.setPosition(transformComponent.position);
        window.draw(renderComponent.sprite);
    }
}
