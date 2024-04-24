#include "MapSystem.h"

#include <iostream>

#include "Coordinator.h"
#include "MapComponent.h"
#include "RenderComponent.h"
#include "RenderSystem.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

    void MapSystem::draw(sf::RenderWindow& window) const {
        for (const auto& entity : m_entities)
        {
            auto& mapComponent = gCoordinator.getComponent<MapComponent>(entity);
            auto& transformComponent = gCoordinator.getComponent<TransformMapComponent>(entity);
            sf::Sprite newSprite = createTile(mapComponent.id, transformComponent.position, transformComponent.rotation, transformComponent.scale);
            window.draw(newSprite);
        }
    }

    sf::Sprite MapSystem::createTile(uint32_t id, sf::Vector2f position, float rotation, float scale) const {
        auto test = texture_atlas.findSubTexture(id);
        auto new_sprite = sf::Sprite(*test.m_texture, test.m_rect);
        new_sprite.setOrigin(new_sprite.getLocalBounds().width / 2, new_sprite.getLocalBounds().height / 2);

        float scaleX = 1.f;
        float scaleY = 1.f;

        new_sprite.setScale(sf::Vector2f(3.0f * scale, 3.0f * scaleY));
        new_sprite.setRotation(new_sprite.getRotation() + rotation);
        new_sprite.setPosition(position.x, position.y); // Multiply position by 16

        return new_sprite;
    }


