#include "ObjectCreatorSystem.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "MultiplayerComponent.h"
#include "Physics.h"
#include "PublicConfigMenager.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

extern PublicConfigSingleton configSingleton;

void ObjectCreatorSystem::update()
{
    for (const auto entity : m_entities)
    {
        const auto& eventInfo = gCoordinator.getComponent<CreateBodyWithCollisionEvent>(entity);

        if (!gCoordinator.hasComponent<ColliderComponent>(eventInfo.entity)) continue;
        if (!gCoordinator.hasComponent<TransformComponent>(eventInfo.entity)) continue;
        if (!gCoordinator.hasComponent<RenderComponent>(eventInfo.entity)) continue;
        // if (configSingleton.GetConfig().debugMode)
        //     std::cout << "[COLLIDER BODY COUNT] " + std::to_string(Physics::getWorld()->GetBodyCount()) << std::endl;

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
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventInfo.entity);
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(eventInfo.entity);
    const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);

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
                configSingleton.GetConfig().gameScale);

        objectPosition.y = convertPixelsToMeters(
            transformComponent.position.y -
            (spriteBounds.height / 2 - (colliderComponent.collision.y + colliderComponent.collision.height / 2)) *
                configSingleton.GetConfig().gameScale);
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
    constexpr auto defaultDensity{1.f};
    constexpr auto defaultFriction{1.f};
    fixtureDef.density = defaultDensity;
    fixtureDef.friction = defaultFriction;
    fixtureDef.filter.categoryBits = config::stringToCategoryBits(eventInfo.tag);
    fixtureDef.filter.maskBits = config::stringToMaskBits(eventInfo.tag);
    fixtureDef.filter.groupIndex = config::stringToIndexGroup(eventInfo.tag);
    fixtureDef.restitution = {0.05f};
    fixtureDef.isSensor = eventInfo.trigger;
    return fixtureDef;
}

b2PolygonShape ObjectCreatorSystem::defineShape(const CreateBodyWithCollisionEvent& eventInfo) const
{
    const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventInfo.entity);
    const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(eventInfo.entity);

    auto spriteBounds = renderComponent.sprite.getGlobalBounds();
    b2PolygonShape boxShape;
    sf::Vector2f objectSize{};

    spriteBounds.height = std::min(spriteBounds.height, 16.f);
    spriteBounds.width = std::min(spriteBounds.width, 16.f);

    if (eventInfo.useTextureSize)
    {
        objectSize.x = convertPixelsToMeters(spriteBounds.width * configSingleton.GetConfig().gameScale) / 2;
        objectSize.y = convertPixelsToMeters(spriteBounds.height * configSingleton.GetConfig().gameScale) / 2;
    }
    else
    {
        objectSize.x =
            convertPixelsToMeters(colliderComponent.collision.width * configSingleton.GetConfig().gameScale) / 2;
        objectSize.y =
            convertPixelsToMeters(colliderComponent.collision.height * configSingleton.GetConfig().gameScale) / 2;
    }

    if (eventInfo.tag == "Weapon")
        boxShape.SetAsBox(objectSize.x, objectSize.y, b2Vec2(0, -objectSize.y), 0);
    else
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

    if (collisionData->tag != "Item") body->SetFixedRotation(true);

    body->CreateFixture(&fixtureDef);
    colliderComponent.body = body;
    colliderComponent.onCollisionEnter = eventInfo.onCollisionEnter;
    colliderComponent.onCollisionOut = eventInfo.onCollisionOut;
    colliderComponent.tag = eventInfo.tag;

    if (collisionData->tag == "Wall" && !gCoordinator.hasComponent<MultiplayerComponent>(eventInfo.entity))
    {
        //TODO jakimś cudem przy dołączania drugiego gracza dwa razy do jakiegoś entity chce dodać multiplayer component,
        // moja opinia jest taka, zę to przez obecność jego przy tworzeniu player entity ale chuj wie
        const auto multiplayerEventComponent = MultiplayerComponent{.type = multiplayerType::ROOM_DIMENSIONS_CHANGED};
        gCoordinator.addComponent(eventInfo.entity, multiplayerEventComponent);
    }

}


void ObjectCreatorSystem::clear()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities) entityToRemove.push_back(entity);

    while (!entityToRemove.empty())
    {
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
    entityToRemove.clear();
    m_entities.clear();
}