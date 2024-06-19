#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window) const
{
    std::vector<std::vector<sf::Sprite>> tiles(config::maximumNumberOfLayers);
    sf::Vector2<unsigned int> windowSize = window.getSize();
    float max_x = 0;
    float max_y = 0;

    for (const auto& entity : m_entities)
    {
        if (auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity);
            layer > 0 && layer < config::maximumNumberOfLayers)
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& collisionComponent = gCoordinator.getComponent<ColliderComponent>(entity);

            sf::FloatRect spriteBounds = sprite.getGlobalBounds();
            max_x = std::max(max_x, transformComponent.position.x);
            max_y = std::max(max_y, transformComponent.position.y);

            sprite.setOrigin(spriteBounds.width / 2.f, spriteBounds.height / 2.f);

            if (collisionComponent.body != nullptr && gCoordinator.hasComponent<PlayerComponent>(entity))
            {
                Collision cc = collisionComponent.collision;

                if (cc.height == 0 || cc.width == 0)
                {
                    cc.height = std::max(spriteBounds.height, 16.f);
                    cc.width = std::max(spriteBounds.width, 16.f);
                    cc.x = 0;
                    cc.y = 0;
                }

                sprite.setOrigin(cc.x + cc.width / 2, cc.y + cc.height / 2);
            }

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
            sprite.setPosition({sprite.getPosition().x, sprite.getPosition().y});
            window.draw(sprite);
        }
    }

    if (config::debugMode)
    {
        debugBoundingBoxes(window);
    }
}

void RenderSystem::debugBoundingBoxes(sf::RenderWindow& window) const
{
    auto renderComponent = gCoordinator.getComponent<RenderComponent>(config::playerEntity);
    auto tileComponent = gCoordinator.getComponent<TileComponent>(config::playerEntity);
    const auto bounds = renderComponent.sprite.getGlobalBounds();

    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);

    Collision cc =
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id);

    float xPosCenter = transformComponent.position.x;
    float yPosCenter = transformComponent.position.y;

    const auto center = GameType::MyVec2{xPosCenter, yPosCenter};

    sf::CircleShape centerPoint(5);
    centerPoint.setFillColor(sf::Color::Red);
    centerPoint.setPosition(center.x, center.y);
    window.draw(centerPoint);

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = center;
    line[0].color = sf::Color::Red;
    line[1].position = {center.x + config::playerAttackRange, center.y};
    line[1].color = sf::Color::Red;
    window.draw(line);

    const b2Vec2 newCenter{convertPixelsToMeters(center.x), convertPixelsToMeters(center.y)};
    const float newRadius = convertPixelsToMeters(config::playerAttackRange);

    b2CircleShape circle;
    circle.m_radius = newRadius;
    circle.m_p = newCenter;

    ////    sf::CircleShape collision(convertMetersToPixel(newRadius));
    //    collision.setOrigin(convertMetersToPixel(newRadius), convertMetersToPixel(newRadius));
    //    collision.setPosition({convertMetersToPixel(newCenter.x), convertMetersToPixel(newCenter.y)});
    //    window.draw(collision);

    for (const auto& entity : gCoordinator.getRegisterSystem<CollisionSystem>()->m_entities)
    {
        if (!gCoordinator.hasComponent<ColliderComponent>(entity) ||
            !gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<RenderComponent>(entity))
        {
            continue;
        };

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (colliderComponent.body == nullptr)
        {
            continue;
        }

        for (b2Fixture* fixture = colliderComponent.body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            b2Shape* shape = fixture->GetShape();
            if (shape->GetType() == b2Shape::e_polygon)
            {
                auto* const polygonShape = dynamic_cast<b2PolygonShape*>(shape);
                const int32 count = polygonShape->m_count;
                sf::ConvexShape convex;
                convex.setPointCount(count);
                for (int32 i = 0; i < count; ++i)
                {
                    const b2Vec2 point = polygonShape->m_vertices[i];
                    convex.setPoint(
                        i, sf::Vector2f(point.x * config::meterToPixelRatio, point.y * config::meterToPixelRatio));
                }
                convex.setFillColor(sf::Color::Transparent);
                convex.setOutlineThickness(1.f);
                convex.setOutlineColor(sf::Color::Green);
                convex.setPosition(colliderComponent.body->GetPosition().x * config::meterToPixelRatio,
                                   colliderComponent.body->GetPosition().y * config::meterToPixelRatio);

                convex.setRotation(colliderComponent.body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}
