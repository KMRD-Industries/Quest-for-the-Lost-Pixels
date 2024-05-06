#pragma once

#include "RenderSystem.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const {
    for (const auto& entity : m_entities) {
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto const& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        renderComponent.sprite.setPosition(transformComponent.position);
        window.draw(renderComponent.sprite);
    }
}