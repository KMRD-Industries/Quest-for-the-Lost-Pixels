
#include "ObjectCreatorSystem.h"

#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

void ObjectCreatorSystem::update(){

    for (const auto& entity : m_entities)
    {
        const auto& eventInfo = gCoordinator.getComponent<CreateBodyWithCollisionEvent>(entity);

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(eventInfo.entity);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventInfo.entity);

        auto* collisionData = new GameType::CollisionData{.entityID = eventInfo.entity, .tag = eventInfo.tag};

        b2BodyDef bodyDef;

        auto spriteBounds = renderComponent.sprite.getGlobalBounds();
        spriteBounds.height = std::min(spriteBounds.height, 16.f);
        spriteBounds.width = std::min(spriteBounds.width, 16.f);

        float xPosition = {};
        float yPosition = {};
        float boxWidth = {};
        float boxHeight = {};

        if (eventInfo.useTextureSize)
        {
            xPosition = convertPixelsToMeters(transformComponent.position.x);
            yPosition = convertPixelsToMeters(transformComponent.position.y);
        }
        else
        {
            xPosition = convertPixelsToMeters(transformComponent.position.x -
                                              (spriteBounds.width / 2 - (eventInfo.offset.x + eventInfo.colliderSize.x / 2)) *
                                                  config::gameScale);

            yPosition = convertPixelsToMeters(transformComponent.position.y -
                                              (spriteBounds.height / 2 - (eventInfo.offset.y + eventInfo.colliderSize.y / 2)) *
                                                  config::gameScale);
        }

        bodyDef.position.Set(xPosition, yPosition);
        bodyDef.angle = transformComponent.rotation;
        bodyDef.type = eventInfo.isStatic ? b2_staticBody : b2_dynamicBody;
        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collisionData);

        b2Body* body = Physics::getWorld()->CreateBody(&bodyDef);
        b2PolygonShape boxShape;

        if (eventInfo.useTextureSize)
        {
            boxWidth = convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2;
            boxHeight = convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2;
        }
        else
        {
            boxWidth = convertPixelsToMeters(eventInfo.colliderSize.x * config::gameScale) / 2;
            boxHeight = convertPixelsToMeters(eventInfo.colliderSize.y * config::gameScale) / 2;
        }

        boxShape.SetAsBox(boxWidth, boxHeight);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &boxShape;
        constexpr auto defaultDensity{1.f};
        constexpr auto defaultFriction{1.f};
        fixtureDef.density = defaultDensity;
        fixtureDef.friction = defaultFriction;
        fixtureDef.filter.categoryBits = 0x0002;
        fixtureDef.filter.maskBits = 0x0002;
        fixtureDef.restitution = {0.05f};

        colliderComponent.fixture = body->CreateFixture(&fixtureDef);
        body->SetFixedRotation(true);

        colliderComponent.body = body;
        colliderComponent.onCollisionEnter = eventInfo.onCollisionEnter;
        colliderComponent.onCollisionOut = eventInfo.onCollisionOut;
        colliderComponent.tag = eventInfo.tag;
    }

    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }

}