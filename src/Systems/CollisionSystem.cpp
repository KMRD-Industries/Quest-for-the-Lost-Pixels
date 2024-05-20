#include "CollisionSystem.h"

#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "Helpers.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

struct RenderComponent;
extern Coordinator gCoordinator;

void CollisionSystem::updateCollision() const
{
    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        if (!transformComponent.velocity.IsValid()) continue;
        b2Body* body = colliderComponent.body;
        body->SetLinearVelocity(transformComponent.velocity);
    }
}
void CollisionSystem::updateSimulation(const float timeStep, const int32 velocityIterations,
                                       const int32 positionIterations)
{
    m_world.Step(timeStep, velocityIterations, positionIterations);
    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        const b2Body* body = colliderComponent.body;
        const auto position = body->GetPosition();
        transformComponent.position = {convertMetersToPixel(position.x), convertMetersToPixel(position.y)};
    }
}

/**
 * \brief Makes 2d box collider for giving entity in our world
 * \param entity The entity for which the physical body should be created.
 * \param colliderSize The size of the collider in pixels (if not using texture size)
 * \param isStatic Specifies whether the body should be static (not moving) or dynamic (moving).
 * \param useTextureSize Specifies whether the collider size should be based on the entity's texture size.
 */
void CollisionSystem::createBody(const Entity entity, const glm::vec2& colliderSize, const bool isStatic,
                                 const bool useTextureSize)
{
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    b2BodyDef bodyDef;
    bodyDef.position.Set(transformComponent.position.x, transformComponent.position.y);
    bodyDef.angle = transformComponent.rotation;

    if (isStatic)
        bodyDef.type = b2_staticBody;
    else
        bodyDef.type = b2_dynamicBody;

    b2Body* body = m_world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;

    if (useTextureSize)
    {
        const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto spriteBounds = renderComponent.sprite.getGlobalBounds();
        boxShape.SetAsBox(convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2,
                          convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2);
    }
    else
        boxShape.SetAsBox(convertPixelsToMeters(colliderSize.x / 2), convertPixelsToMeters(colliderSize.y / 2));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    constexpr auto defaultDensity{1.f};
    constexpr auto defaultFriction{0.3f};
    fixtureDef.density = defaultDensity;
    fixtureDef.friction = defaultFriction;
    fixtureDef.filter.categoryBits = 0x0002;
    fixtureDef.filter.maskBits = 0x0002;

    body->CreateFixture(&fixtureDef);

    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
    colliderComponent.body = body;
}