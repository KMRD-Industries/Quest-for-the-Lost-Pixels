#include "CollisionSystem.h"
#include <iostream>
#include "ColliderComponent.h"
#include "DoorComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MyContactListener::BeginContact(b2Contact* contact)
{
    const auto bodyAData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);

    const auto bodyBData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if ((bodyAData != nullptr) && (bodyBData != nullptr))
    {
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entityID);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entityID);
        colliderComponentA.onCollisionEnter({bodyBData->entityID, bodyBData->tag});
        colliderComponentB.onCollisionEnter({bodyAData->entityID, bodyAData->tag});
    }
}

void MyContactListener::EndContact(b2Contact* contact)
{
    const auto bodyAData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);

    const auto bodyBData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if ((bodyAData != nullptr) && (bodyBData != nullptr))
    {
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entityID);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entityID);
        colliderComponentA.onCollisionOut({bodyBData->entityID, bodyBData->tag});
        colliderComponentB.onCollisionOut({bodyAData->entityID, bodyAData->tag});
    }
}

void CollisionSystem::createMapCollision() const
{
    for (const auto& entity : m_entities)
    {
        if (!gCoordinator.hasComponent<TileComponent>(entity) || gCoordinator.hasComponent<PlayerComponent>(entity) ||
            gCoordinator.hasComponent<DoorComponent>(entity))
        {
            continue;
        }

        deleteBody(entity);
    }

    for (const auto& entity : m_entities)
    {
        if (gCoordinator.hasComponent<PlayerComponent>(entity)) continue;

        if (auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
            tileComponent.tileset == "SpecialBlocks")
            if (!gCoordinator.hasComponent<TileComponent>(entity)) continue;

        if (auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
            tileComponent.tileset == "SpecialBlocks")
        {
            if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::STATICWALLCOLLIDER) + 1)
                createBody(
                    entity, "Wall", {config::tileHeight, config::tileHeight},
                    [](const GameType::CollisionData& entityT) {}, [](const GameType::CollisionData& entityT) {}, true,
                    true);
            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER) + 1)
            {
                createBody(
                    entity, "Door", {config::tileHeight, config::tileHeight},
                    [](const GameType::CollisionData& entityT) {}, [](const GameType::CollisionData& entityT) {}, true,
                    false);
            }
        }
        else if (Collision collision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(
                     tileComponent.tileset, tileComponent.id);
                 collision.width > 0 && collision.height > 0)
        {
            createBody(
                entity, "Wall", {collision.width, collision.height}, [](const GameType::CollisionData& entityT) {},
                [](const GameType::CollisionData& entityT) {}, true, false, {collision.x, collision.y});
        }
    }
}

void CollisionSystem::updateCollision() const
{
    for (const auto& entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        if (!transformComponent.velocity.IsValid()) continue;

        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;

        //        correctPosition(entity, body, transformComponent);

        body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                 convertPixelsToMeters(transformComponent.velocity.y)});
    }
}

void CollisionSystem::updateSimulation(const float timeStep, const int32 velocityIterations,
                                       const int32 positionIterations) const
{
    Physics::getWorld()->Step(timeStep, velocityIterations, positionIterations);
    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

        Collision collision = colliderComponent.collision;
        //            gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset,
        //            tileComponent.id);

        const b2Body* body = colliderComponent.body;
        if (body == nullptr || transformComponent.velocity == b2Vec2{}) continue;

        const auto position = body->GetPosition();
        const auto [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity);
        const auto spriteBounds = sprite.getGlobalBounds();
        int mulForBlocks = 1;

        if (collision.height == 0 || collision.width == 0)
        {
            collision.height = std::max(spriteBounds.height, 16.f);
            collision.width = std::max(spriteBounds.width, 16.f);
            collision.x = 0;
            collision.y = 0;
        }

        if (gCoordinator.hasComponent<TileComponent>(entity)) mulForBlocks = config::gameScale;

        transformComponent.position = {static_cast<float>(convertMetersToPixel(position.x)),
                                       static_cast<float>(convertMetersToPixel(position.y))};
    }
}

/**
 * \brief Makes 2d box collider for giving entity in our world
 * \param entity The entity for which the physical body should be created.
 * \param colliderSize The size of the collider in pixels (if not using texture size)
 * \param onCollisionEnter Callback to function run on enter collision.
 * \param onCollisionOut Callback to function run on leave collision.
 * \param isStatic Specifies whether the body should be static (not moving) or dynamic (moving).
 * \param useTextureSize Specifies whether the collider size should be based on the entity's texture size.
 */
void CollisionSystem::createBody(const Entity entity, const std::string& tag, const glm::vec2& colliderSize,
                                 const std::function<void(GameType::CollisionData)>& onCollisionEnter,
                                 const std::function<void(GameType::CollisionData)>& onCollisionOut,
                                 const bool isStatic, const bool useTextureSize, const glm::vec2& offset)
{
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

    b2BodyDef bodyDef;

    sf::Sprite sprite =
        gCoordinator.getRegisterSystem<TextureSystem>().get()->getTile(tileComponent.tileset, tileComponent.id);

    if (useTextureSize)
    {
        float xPosition = convertPixelsToMeters(transformComponent.position.x);
        float yPosition = convertPixelsToMeters(transformComponent.position.y);


        //        float xPosition = convertPixelsToMeters(transformComponent.position.x +
        //                                                (sprite.getTextureRect().width - 16) * config::gameScale);
        //
        //        float yPosition = convertPixelsToMeters(transformComponent.position.y +
        //                                                (sprite.getTextureRect().height - 16) * config::gameScale);

        bodyDef.position.Set(xPosition, yPosition);
    }
    else
    {
        //        float xPosition =
        //            convertPixelsToMeters(transformComponent.position.x + (offset.x - colliderSize.x / 2) *
        //            config::gameScale);
        //
        //        float yPosition = convertPixelsToMeters(transformComponent.position.y + (offset.y) * config::gameScale
        //        / 2);

        float xPosition = convertPixelsToMeters(transformComponent.position.x -
                                                (sprite.getGlobalBounds().width / 2 - (offset.x + colliderSize.x / 2)) *
                                                    config::gameScale);
        float yPosition = convertPixelsToMeters(
            transformComponent.position.y -
            (sprite.getGlobalBounds().height / 2 - (offset.y + colliderSize.y / 2)) * config::gameScale);

        bodyDef.position.Set(xPosition, yPosition);
    }

    bodyDef.angle = transformComponent.rotation;
    bodyDef.type = (isStatic) ? b2_staticBody : b2_dynamicBody;

    auto* collisionData = new GameType::CollisionData{.entityID = entity, .tag = tag};
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collisionData);
    b2Body* body = Physics::getWorld()->CreateBody(&bodyDef);

    b2PolygonShape boxShape;

    if (useTextureSize)
    {
        //        const auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity);
        const auto spriteBounds = sprite.getGlobalBounds();
        boxShape.SetAsBox(convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2,
                          convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2);
    }
    else
    {
        // Set the collision box size
        float boxWidth = convertPixelsToMeters(colliderSize.x * config::gameScale) / 2.f;
        float boxHeight = convertPixelsToMeters(colliderSize.y * config::gameScale) / 2.f;

        boxShape.SetAsBox(boxWidth, boxHeight);
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    constexpr auto defaultDensity{1.f};
    constexpr auto defaultFriction{1.f};
    fixtureDef.density = defaultDensity;
    fixtureDef.friction = defaultFriction;
    fixtureDef.filter.categoryBits = 0x0002;
    fixtureDef.filter.maskBits = 0x0002;

    body->CreateFixture(&fixtureDef);
    body->SetFixedRotation(true);

    colliderComponent.body = body;
    colliderComponent.onCollisionEnter = onCollisionEnter;
    colliderComponent.onCollisionOut = onCollisionOut;
    colliderComponent.tag = tag;
}


void CollisionSystem::deleteBody(Entity entity)
{
    if (!gCoordinator.hasComponent<ColliderComponent>(entity)) return;
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
    if (colliderComponent.body != nullptr) Physics::getWorld()->DestroyBody(colliderComponent.body);
    colliderComponent.body = nullptr;
    colliderComponent.collision = {};
}

void CollisionSystem::deleteMarkedBodies() const
{
    std::unordered_set<Entity> entityToKill{};

    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        if (!colliderComponent.toDestroy) continue;
        deleteBody(entity);
        entityToKill.insert(entity);
    }

    for (auto& entity : entityToKill) gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}
