#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TransformComponent.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_shape.h"
#include "box2d/b2_api.h"
#include "box2d/b2_polygon_shape.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    if (config::debugBoundingBox)
        debugBoundingBoxes(window);

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

void RenderSystem::debugBoundingBoxes(sf::RenderWindow& window) const
{
    for (b2Body* body = Physics::getWorld()->GetBodyList(); body; body = body->GetNext())
    {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Shape* shape = fixture->GetShape();
            if (shape->GetType() == b2Shape::e_polygon)
            {
                const auto polygonShape = reinterpret_cast<b2PolygonShape*>(shape);
                const int32 count = polygonShape->m_count;
                sf::ConvexShape convex;
                convex.setPointCount(count);
                for (int32 i = 0; i < count; ++i)
                {
                    const b2Vec2 point = polygonShape->m_vertices[i];
                    convex.setPoint(i, sf::Vector2f(point.x * config::meterToPixelRatio,
                                                    point.y * config::meterToPixelRatio));
                }
                convex.setFillColor(sf::Color::Transparent);
                convex.setOutlineThickness(1.f);
                convex.setOutlineColor(sf::Color::Green);
                convex.setPosition(body->GetPosition().x * config::meterToPixelRatio,
                                   body->GetPosition().y * config::meterToPixelRatio);
                convex.setRotation(body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}
