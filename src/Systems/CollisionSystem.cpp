#include "CollisionSystem.h"
#include <iostream>
#include "ColliderComponent.h"
#include "Config.h"
#include "DoorComponent.h"
#include "Helpers.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void MyContactListener::BeginContact(b2Contact* contact)
{
    auto* const bodyAData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
    auto* const bodyBData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if ((bodyAData != nullptr) && (bodyBData != nullptr))
    {
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entityID);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entityID);

        colliderComponentA.collisionReaction({bodyBData->entityID, bodyBData->tag});
        colliderComponentB.collisionReaction({bodyAData->entityID, bodyAData->tag});
    }
}


void MyContactListener::EndContact(b2Contact* contact) { std::cout << "Stop collision\n"; }

void CollisionSystem::updateCollision()
{
    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        if (!transformComponent.velocity.IsValid())
        {
            continue;
        }

        b2Body* body = colliderComponent.body;

        if (body == nullptr)
        {
            continue;
        }
        body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                 convertPixelsToMeters(transformComponent.velocity.y)});
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
        if (body == nullptr) continue;
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
 * \param offset Offset of texture collision
 */
void CollisionSystem::createBody(Entity entity, const std::string& tag, const glm::vec2& colliderSize,
                                 const std::function<void(GameType::CollisionData)>& collisionReaction, bool isStatic,
                                 bool useTextureSize, const glm::vec2& offset)
{
    const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

    b2BodyDef bodyDef;
    bodyDef.position.Set(convertPixelsToMeters(transformComponent.position.x),
                         convertPixelsToMeters(transformComponent.position.y));
    bodyDef.angle = transformComponent.rotation;

    if (isStatic)
        bodyDef.type = b2_staticBody;
    else
        bodyDef.type = b2_dynamicBody;

    auto* collisionData = new GameType::CollisionData{.entityID = entity, .tag = tag};

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collisionData);
    b2Body* body = m_world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;

    if (useTextureSize)
    {
        const auto& [sprite, layer] = gCoordinator.getComponent<RenderComponent>(entity);
        const auto spriteBounds = sprite.getGlobalBounds();
        boxShape.SetAsBox(convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2,
                          convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2);
    }
    else
    {
        // Set the collision box size
        float boxWidth = convertPixelsToMeters(colliderSize.x * config::gameScale) / 2;
        float boxHeight = convertPixelsToMeters(colliderSize.y * config::gameScale) / 2;

        // Set the collision box position relative to the entity's position
        float offsetX = convertPixelsToMeters((offset.x + colliderSize.x / 2.f * config::gameScale));
        float offsetY = convertPixelsToMeters((offset.y + colliderSize.y / 2.f * config::gameScale));

        boxShape.SetAsBox(boxWidth, boxHeight, b2Vec2(offsetX, offsetY), 0);
    }
    //    boxShape.SetAsBox(convertPixelsToMeters((colliderSize.x + offset.x) * config::gameScale / 2),
    //                      convertPixelsToMeters((colliderSize.y + offset.y) * config::gameScale / 2));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    constexpr auto defaultDensity{1.f};
    constexpr auto defaultFriction{0.f};
    fixtureDef.density = defaultDensity;
    fixtureDef.friction = defaultFriction;
    fixtureDef.filter.categoryBits = 0x0002;
    fixtureDef.filter.maskBits = 0x0002;

    body->CreateFixture(&fixtureDef);
    body->SetFixedRotation(true);
    colliderComponent.body = body;
    colliderComponent.collisionReaction = collisionReaction;
    colliderComponent.tag = tag;
}

void CollisionSystem::deleteBody(Entity entity)
{
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

    // Do not remove player collisions
    if (gCoordinator.hasComponent<PlayerComponent>(entity))
    {
        return;
    }

    if (colliderComponent.body != nullptr)
    {
        m_world.DestroyBody(colliderComponent.body);
    }
}

void CollisionSystem::resetCollisions()
{
    m_world.ClearForces();

    for (const auto& entity : m_entities)
    {
        if (gCoordinator.hasComponent<DoorComponent>(entity))
        {
            gCoordinator.removeComponent<DoorComponent>(entity);
        }

        deleteBody(entity);
    }
}

// Similar to loadTextures
// Given entities from mapLoader set up collisions
void CollisionSystem::loadCollisions()
{
    resetCollisions();

    for (const auto& entity : m_entities)
    {
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

        if (gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            continue;
        }

        Collision collision = colliderComponent.collisionDescription;
        //            gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset,
        //            tileComponent.id);

        if (collision.width <= 0 || collision.height <= 0)
        {
            continue;
        }

        // Doors Collider
        if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER) + 1 &&
            tileComponent.tileset == "SpecialBlocks")
        {
            createBody(
                entity, "Door", {collision.width, collision.height}, [](const GameType::CollisionData& entityT) {},
                true, true);

            if (!gCoordinator.hasComponent<DoorComponent>(entity))
            {
                gCoordinator.addComponent(entity, DoorComponent{});
            }

            auto& doorComponent = gCoordinator.getComponent<DoorComponent>(entity);

            if (transformComponent.position.y == 0)
                doorComponent.entrance = GameType::DoorEntraces::NORTH;
            else if (transformComponent.position.y > 0)
                doorComponent.entrance = GameType::DoorEntraces::SOUTH;

            if (transformComponent.position.x == 0)
                doorComponent.entrance = GameType::DoorEntraces::WEST;
            else if (transformComponent.position.x > 0)
                doorComponent.entrance = GameType::DoorEntraces::EAST;
        }
        else
        {
            // Static wall collider
            createBody(entity, "Wall", {collision.width, collision.height},
                       [](const GameType::CollisionData& entityT) {}, true, false, {collision.x, collision.y});
        }
    }
}
