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
    for (const auto entity : m_entities)
    {
        if (!gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            deleteBody(entity);
        }
    }

    auto createCollisionBody = [this](const Entity entity, const std::string& type, const glm::vec2& size,
                                      const bool isStatic, const bool useTexture, const glm::vec2& offset = {0, 0})
    {
        const Entity newEntity = gCoordinator.createEntity();
        const auto newEvent = CreateBodyWithCollisionEvent(
            entity, type, [](const GameType::CollisionData&) {}, [](const GameType::CollisionData&) {}, isStatic,
            useTexture);

        gCoordinator.addComponent(newEntity, newEvent);
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
                createCollisionBody(entity, "Wall", {config::tileHeight, config::tileHeight}, true, true);

            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER))
                createCollisionBody(entity, "Door", {config::tileHeight, config::tileHeight}, true, false);

            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOWNDOOR))
                if (gCoordinator.hasComponent<PassageComponent>(entity))
                    if (gCoordinator.getComponent<PassageComponent>(entity).activePassage)
                        createCollisionBody(entity, "Passage", {config::tileHeight, config::tileHeight}, true, true);
        }
        else
        {
            if (colliderComponent.collision.width > 0 && colliderComponent.collision.height > 0)
                createCollisionBody(entity, "Wall",
                                    {colliderComponent.collision.width, colliderComponent.collision.height}, true,
                                    false, {colliderComponent.collision.x, colliderComponent.collision.y});
        }
    }
}

void CollisionSystem::update()
{
    for (const auto entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (!transformComponent.velocity.IsValid()) continue;

        b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;

        body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                 convertPixelsToMeters(transformComponent.velocity.y)});

        transformComponent.velocity = {};
    }
}

void CollisionSystem::updateSimulation(const float timeStep, const int32 velocityIterations,
                                       const int32 positionIterations) const
{
    Physics::getWorld()->Step(timeStep, velocityIterations, positionIterations);

    for (const auto entity : m_entities)
    {
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);

        const b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;
        if (colliderComponent.tag != "Player 1" && colliderComponent.tag != "SecondPlayer" &&
            colliderComponent.tag != "Enemy" && colliderComponent.tag != "Item")
            continue;

        const auto position = body->GetPosition();
        transformComponent.position = {convertMetersToPixel(position.x), convertMetersToPixel(position.y)};

        renderComponent.sprite.setPosition(position.x, position.y);
    }
}

void CollisionSystem::deleteBody(const Entity entity)
{
    if (!gCoordinator.hasComponent<ColliderComponent>(entity)) return;
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);
    if (colliderComponent.body != nullptr) Physics::getWorld()->DestroyBody(colliderComponent.body);
    colliderComponent.body = nullptr;
    colliderComponent.collision = {};
}

void CollisionSystem::deleteMarkedBodies()
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
