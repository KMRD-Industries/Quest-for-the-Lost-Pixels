#include "CollisionSystem.h"

#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "DoorComponent.h"
#include "ItemComponent.h"
#include "MultiplayerSystem.h"
#include "PassageComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;

void MyContactListener::BeginContact(b2Contact* contact)
{
    const auto bodyAData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);

    const auto bodyBData =
        reinterpret_cast<GameType::CollisionData*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

    if (bodyAData != nullptr && bodyBData != nullptr)
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

    if (bodyAData != nullptr && bodyBData != nullptr)
    {
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entityID);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entityID);
        colliderComponentA.onCollisionOut({bodyBData->entityID, bodyBData->tag});
        colliderComponentB.onCollisionOut({bodyAData->entityID, bodyAData->tag});
    }
}

CollisionSystem::CollisionSystem() { init(); }

void CollisionSystem::init() { Physics::getWorld()->SetContactListener(&m_myContactListenerInstance); }

void CollisionSystem::createMapCollision()
{
    auto createCollisionBody =
        [this](const Entity entity, const std::string& type, const bool isStatic, const bool useTexture)
    {
        const Entity newMapCollisionEntity = gCoordinator.createEntity();

        const auto newEvent = CreateBodyWithCollisionEvent(
            entity, type, [](const GameType::CollisionData&) {}, [](const GameType::CollisionData&) {}, isStatic,
            useTexture);

        gCoordinator.addComponent(newMapCollisionEntity, newEvent);
    };

    for (const auto entity : m_entities)
    {
        const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (tileComponent.id < 0 || tileComponent.tileSet.empty() ||
            gCoordinator.hasComponent<PlayerComponent>(entity) || gCoordinator.hasComponent<WeaponComponent>(entity))
        {
            continue;
        }

        if (tileComponent.tileSet == "SpecialBlocks")
        {
            if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::STATICWALLCOLLIDER))
                createCollisionBody(entity, "Wall", true, true);

            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER))
                createCollisionBody(entity, "Door", true, false);

            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOWNDOOR))
                if (const auto* passageComponent = gCoordinator.tryGetComponent<PassageComponent>(entity))
                    if (passageComponent->activePassage) createCollisionBody(entity, "Passage", true, true);
        }
        else
        {
            if (colliderComponent.collision.width > 0 && colliderComponent.collision.height > 0)
                createCollisionBody(entity, "Wall", true, false);
        }
    }
}

void CollisionSystem::update() const
{
    for (const auto entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        if (!transformComponent.velocity.IsValid()) continue;

        b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;

        if (colliderComponent.tag == "Item")
        {
            body->ApplyForceToCenter({transformComponent.velocity.x, transformComponent.velocity.y}, true);
        }
        else
        {
            body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                     convertPixelsToMeters(transformComponent.velocity.y)});
        }

        renderComponent.dirty = true;
        transformComponent.velocity = {};
    }
}

void CollisionSystem::updateSimulation(const float timeStep, const int32 velocityIterations,
                                       const int32 positionIterations) const
{
    if (config::debugMode)
        std::cout << "[COLLIDER BODY COUNT] " + std::to_string(Physics::getWorld()->GetBodyCount()) << std::endl;

    Physics::getWorld()->Step(timeStep, velocityIterations, positionIterations);

    for (const auto entity : m_entities)
    {
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        const b2Body* body = colliderComponent.body;
        if (body == nullptr || transformComponent.velocity == b2Vec2{}) continue;
        if (!colliderComponent.tag.starts_with("Player") && colliderComponent.tag != "Enemy") continue;

        const auto position = body->GetPosition();

        transformComponent.position = {convertMetersToPixel(position.x), convertMetersToPixel(position.y)};
        transformComponent.rotation = body->GetAngle() * 180.f / 3.131516;
        renderComponent.sprite.setPosition(position.x, position.y);
        renderComponent.dirty = true;
    }
}

void CollisionSystem::deleteBody(const Entity entity) const
{
    if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
    {
        if (colliderComponent->body != nullptr) Physics::getWorld()->DestroyBody(colliderComponent->body);
        colliderComponent->body = nullptr;
        colliderComponent->collision = {};
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

    auto* collisionData = new GameType::CollisionData{.entityID = entity, .tag = tag};

    b2BodyDef bodyDef;
    sf::Sprite sprite =
        gCoordinator.getRegisterSystem<TextureSystem>().get()->getTile(tileComponent.tileset, tileComponent.id);
    const auto spriteBounds = sprite.getGlobalBounds();

    float xPosition = {};
    float yPosition = {};
    float boxWidth = {};
    float boxHeight = {};

    if (useTextureSize)
    {
        xPosition = convertPixelsToMeters(transformComponent.position.x);
        yPosition = convertPixelsToMeters(transformComponent.position.y);
    }
    else
    {
        xPosition = convertPixelsToMeters(transformComponent.position.x -
                                          (sprite.getGlobalBounds().width / 2 - (offset.x + colliderSize.x / 2)) *
                                              config::gameScale);
        yPosition = convertPixelsToMeters(transformComponent.position.y -
                                          (sprite.getGlobalBounds().height / 2 - (offset.y + colliderSize.y / 2)) *
                                              config::gameScale);
    }

    bodyDef.position.Set(xPosition, yPosition);
    bodyDef.angle = transformComponent.rotation;
    bodyDef.type = (isStatic) ? b2_staticBody : b2_dynamicBody;
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collisionData);

    b2Body* body = Physics::getWorld()->CreateBody(&bodyDef);
    b2PolygonShape boxShape;

    if (useTextureSize)
    {
        boxWidth = convertPixelsToMeters(spriteBounds.width * config::gameScale) / 2;
        boxHeight = convertPixelsToMeters(spriteBounds.height * config::gameScale) / 2;
    }
    else
    {
        boxWidth = convertPixelsToMeters(colliderSize.x * config::gameScale) / 2;
        boxHeight = convertPixelsToMeters(colliderSize.y * config::gameScale) / 2;
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

    body->CreateFixture(&fixtureDef);
    body->SetFixedRotation(true);

    // set remote players' body to static so that players can't move each other
    if (gCoordinator.hasComponent<MultiplayerComponent>(entity)) body->SetType(b2BodyType::b2_staticBody);

    colliderComponent.body = body;
    colliderComponent.onCollisionEnter = onCollisionEnter;
    colliderComponent.onCollisionOut = onCollisionOut;
    colliderComponent.tag = tag;
}

void CollisionSystem::deleteBody(const Entity entity)
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
