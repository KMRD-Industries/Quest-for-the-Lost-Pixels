#include "ObjectCreatorSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

void ObjectCreatorSystem::update()
{
    for (const auto entity : m_entities)
    {
        const auto& eventInfo = gCoordinator.getComponent<CreateBodyWithCollisionEvent>(entity);

        if (!gCoordinator.hasComponent<ColliderComponent>(eventInfo.entity)) continue;
        if (!gCoordinator.hasComponent<TransformComponent>(eventInfo.entity)) continue;
        if (!gCoordinator.hasComponent<RenderComponent>(eventInfo.entity)) continue;

        switch (eventInfo.type)
        {
        case GameType::ObjectType::NORMAL:
            createBasicObject(eventInfo);
            break;
        case GameType::ObjectType::BULLET:
            // TODO: Bullets support
            createBasicObject(eventInfo);
            break;
        default:
            break;
        }
    }

    clear();
}

b2BodyDef ObjectCreatorSystem::defineBody(const CreateBodyWithCollisionEvent& eventInfo) const
{
    const auto& renderComponent =           gCoordinator.getComponent<RenderComponent>(eventInfo.entity);
    const auto& transformComponent =        gCoordinator.getComponent<TransformComponent>(eventInfo.entity);
    const auto& colliderComponent =         gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);

    b2BodyDef bodyDef;

    auto spriteBounds = renderComponent.sprite.getGlobalBounds();
    spriteBounds.height = std::min(spriteBounds.height, 16.f);
    spriteBounds.width = std::min(spriteBounds.width, 16.f);

    sf::Vector2f objectPosition{};

    // Calculate position of object
    if (eventInfo.useTextureSize)
    {
        objectPosition.x = convertPixelsToMeters(transformComponent.position.x);
        objectPosition.y = convertPixelsToMeters(transformComponent.position.y);
    }
    else
    {
        objectPosition.x = convertPixelsToMeters(
            transformComponent.position.x -
            (spriteBounds.width / 2.f - (colliderComponent.collision.x + colliderComponent.collision.width / 2.f)) *
            config::gameScale);

        objectPosition.y = convertPixelsToMeters(
            transformComponent.position.y -
            (spriteBounds.height / 2 - (colliderComponent.collision.y + colliderComponent.collision.height / 2)) *
            config::gameScale);
    }

    bodyDef.position.Set(objectPosition.x, objectPosition.y);
    bodyDef.angle = transformComponent.rotation * (M_PI / 180.f);
    bodyDef.type = eventInfo.isStatic ? b2_staticBody : b2_dynamicBody;
    bodyDef.bullet = eventInfo.type == GameType::ObjectType::BULLET;
    return bodyDef;
}

b2FixtureDef ObjectCreatorSystem::defineFixture(const CreateBodyWithCollisionEvent& eventInfo) const
{
    b2FixtureDef fixtureDef;

    // DEFAULT PHYSIC BEHAVIOUR
    fixtureDef.density = config::defaultDensity;
    fixtureDef.friction = config::defaultFriction;
    fixtureDef.restitution = config::defaultRestitution;

    // COLLISIONS FILTERING SETUP
    fixtureDef.filter.categoryBits = config::stringToCategoryBits(eventInfo.tag);
    fixtureDef.filter.maskBits = config::stringToMaskBits(eventInfo.tag);
    fixtureDef.filter.groupIndex = config::stringToIndexGroup(eventInfo.tag);

    return fixtureDef;
}

b2PolygonShape ObjectCreatorSystem::defineShape(const CreateBodyWithCollisionEvent& eventInfo) const
{
    const auto& renderComponent =       gCoordinator.getComponent<RenderComponent>(eventInfo.entity);
    const auto& colliderComponent =     gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);

    auto spriteBounds = renderComponent.sprite.getGlobalBounds();
    b2PolygonShape boxShape;
    sf::Vector2f objectSize {};

    spriteBounds.height = std::min(spriteBounds.height, config::tileHeight);
    spriteBounds.width = std::min(spriteBounds.width, config::tileHeight);

    if (eventInfo.useTextureSize)
    {
        objectSize.x = convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2;
        objectSize.y = convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2;
    }
    else
    {
        objectSize.x = convertPixelsToMeters(colliderComponent.collision.width * config::gameScale) / 2;
        objectSize.y = convertPixelsToMeters(colliderComponent.collision.height * config::gameScale) / 2;
    }

    boxShape.SetAsBox(objectSize.x, objectSize.y);
    return boxShape;
}


void ObjectCreatorSystem::createBasicObject(const CreateBodyWithCollisionEvent& eventInfo) const
{
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);
    auto* collisionData = new GameType::CollisionData{.entityID = eventInfo.entity, .tag = eventInfo.tag};

    // Create all components
    const b2PolygonShape shape = defineShape(eventInfo);
    b2BodyDef bodyDef = defineBody(eventInfo);
    b2FixtureDef fixtureDef = defineFixture(eventInfo);

    fixtureDef.shape = &shape;
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collisionData);

    b2Body* body = Physics::getWorld()->CreateBody(&bodyDef);

    if(collisionData->tag != "Item")
        body->SetFixedRotation(true);

    body->CreateFixture(&fixtureDef);
    colliderComponent.body = body;
    colliderComponent.onCollisionEnter = eventInfo.onCollisionEnter;
    colliderComponent.onCollisionOut = eventInfo.onCollisionOut;
    colliderComponent.tag = eventInfo.tag;
}


void ObjectCreatorSystem::clear()
{
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
    entityToRemove.clear();
    m_entities.clear();
}