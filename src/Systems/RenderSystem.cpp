#include "RenderSystem.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;

void RenderSystem::draw(sf::RenderWindow& window)
{
    std::vector<std::vector<sf::Sprite>> tiles(config::maximumNumberOfLayers);
    sf::Vector2<unsigned int> windowSize = window.getSize();

    mapRenderOffsetX = 0;
    mapRenderOffsetY = 0;

    for (const auto entity : m_entities)
    {
        if (auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity);
            layer > 0 && layer < config::maximumNumberOfLayers)
        {
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
            auto& collisionComponent = gCoordinator.getComponent<ColliderComponent>(entity);

            sf::FloatRect spriteBounds = sprite.getGlobalBounds();
            mapRenderOffsetX = std::max(mapRenderOffsetX, transformComponent.position.x);
            mapRenderOffsetY = std::max(mapRenderOffsetY, transformComponent.position.y);

            sprite.setOrigin(spriteBounds.width / 2.f, spriteBounds.height / 2.f);

            if (collisionComponent.collision.height == 0 || collisionComponent.collision.width == 0)
            {
                collisionComponent.collision.height = std::max(spriteBounds.height, 16.f);
                collisionComponent.collision.width = std::max(spriteBounds.width, 16.f);
                collisionComponent.collision.x = 0;
                collisionComponent.collision.y = 0;
            }

            sprite.setOrigin(collisionComponent.collision.x + collisionComponent.collision.width / 2,
                             collisionComponent.collision.y + collisionComponent.collision.height / 2);

            sprite.setScale(transformComponent.scale * config::gameScale);
            sprite.setPosition(transformComponent.position);
            sprite.setRotation(transformComponent.rotation);

            tiles[layer].push_back(sprite);
        }
    }

    mapRenderOffsetX = (windowSize.x - mapRenderOffsetX) * 0.5f;
    mapRenderOffsetY = (windowSize.y - mapRenderOffsetY) * 0.5f;

    for (auto& layer : tiles)
    {
        for (auto& sprite : layer)
        {
            sprite.setPosition({sprite.getPosition().x + mapRenderOffsetX, sprite.getPosition().y + mapRenderOffsetY});
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
    auto transformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);

    auto drawSprite = [&](Entity entity)
    {
        auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto bounds = renderComponent.sprite.getGlobalBounds();

        sf::ConvexShape convex;
        convex.setPointCount(4);
        convex.setPoint(0, {0, 0});
        convex.setPoint(1, {bounds.width, 0});
        convex.setPoint(2, {bounds.width, bounds.height});
        convex.setPoint(3, {0, bounds.height});

        convex.setFillColor(sf::Color::Transparent);
        convex.setOutlineThickness(1.f);
        convex.setOutlineColor(sf::Color::Yellow);

        const auto spriteCenter =
            GameType::MyVec2{transformComponent.position.x + mapRenderOffsetX - bounds.width / 2,
                             transformComponent.position.y + mapRenderOffsetY - bounds.height / 2};
        convex.setPosition(spriteCenter);

        window.draw(convex);
    };

    Collision cc =
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id);

    float xPosCenter = transformComponent.position.x;
    float yPosCenter = transformComponent.position.y;

    const auto center = GameType::MyVec2{xPosCenter + mapRenderOffsetX, yPosCenter + mapRenderOffsetY};

    sf::CircleShape centerPoint(5);
    centerPoint.setFillColor(sf::Color::Red);
    centerPoint.setPosition(center.x, center.y);
    window.draw(centerPoint);

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = center;
    line[0].color = sf::Color::Red;
    line[1].position = {center.x + config::playerAttackRange * transformComponent.scale.x, center.y};
    line[1].color = sf::Color::Red;
    window.draw(line);

    const b2Vec2 newCenter{convertPixelsToMeters(center.x), convertPixelsToMeters(center.y)};
    const float newRadius = convertPixelsToMeters(config::playerAttackRange);

    b2CircleShape circle;
    circle.m_radius = newRadius;
    circle.m_p = newCenter;

    //    sf::CircleShape collision(convertMetersToPixel(newRadius));
    //    collision.setOrigin(convertMetersToPixel(newRadius), convertMetersToPixel(newRadius));
    //    collision.setPosition({convertMetersToPixel(newCenter.x), convertMetersToPixel(newCenter.y)});
    //    window.draw(collision);

    for (const auto entity : gCoordinator.getRegisterSystem<CollisionSystem>()->m_entities)
    {
        if (!gCoordinator.hasComponent<ColliderComponent>(entity) ||
            !gCoordinator.hasComponent<TransformComponent>(entity) ||
            !gCoordinator.hasComponent<RenderComponent>(entity))
        {
            continue;
        };

        if (gCoordinator.hasComponent<PlayerComponent>(entity) || gCoordinator.hasComponent<EnemyComponent>(entity))
        {
            drawSprite(entity);
        }

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
                convex.setPosition(
                    colliderComponent.body->GetPosition().x * config::meterToPixelRatio + mapRenderOffsetX,
                    colliderComponent.body->GetPosition().y * config::meterToPixelRatio + mapRenderOffsetY);

                convex.setRotation(colliderComponent.body->GetAngle() * 180 / b2_pi);
                window.draw(convex);
            }
        }
    }
}
