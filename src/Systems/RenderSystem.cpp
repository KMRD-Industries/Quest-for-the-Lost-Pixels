#include "RenderSystem.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    for (const auto& entity : m_entities)
    {
        auto const& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        window.draw(*renderComponent.shape);
    }
}
