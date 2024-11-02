#include "BackgroundSystem.h"
#include "Coordinator.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"
#include "UiComponent.h"

extern Coordinator gCoordinator;

void BackgroundSystem::draw(sf::RenderWindow& window)
{
    for (const auto entity : m_entities)
    {
        auto& uiComponent = gCoordinator.getComponent<UiComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        uiComponent.sprite->setPosition(transformComponent.position);
        uiComponent.sprite->setRotation(transformComponent.rotation);
        uiComponent.sprite->setScale(transformComponent.scale);

        window.draw(*uiComponent.sprite);
    }
}