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

CollisionSystem::CollisionSystem()
{
    init();
}

void CollisionSystem::init()
{
    Physics::getWorld()->SetContactListener(&m_myContactListenerInstance);
}

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
                    if (passageComponent->activePassage)
                        createCollisionBody(entity, "Passage", true, true);
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
        auto& transformComponent    = gCoordinator.getComponent<TransformComponent>(entity);
        auto& colliderComponent     = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& renderComponent       = gCoordinator.getComponent<RenderComponent>(entity);

        if (!transformComponent.velocity.IsValid()) continue;

        b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;

        if (colliderComponent.tag == "Item" ||  colliderComponent.tag == "Bullet")
        {
            body->ApplyForceToCenter({transformComponent.velocity.x, transformComponent.velocity.y}, true);
        }
        else
        {
            body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                     convertPixelsToMeters(transformComponent.velocity.y)});
        }

        renderComponent.dirty = true;
        if(colliderComponent.tag != "Bullet") transformComponent.velocity = {};
    }
}

void CollisionSystem::updateSimulation(const float timeStep, const int32 velocityIterations,
                                       const int32 positionIterations) const
{
    Physics::getWorld()->Step(timeStep, velocityIterations, positionIterations);

    for (const auto entity : m_entities)
    {
        auto& colliderComponent     = gCoordinator.getComponent<ColliderComponent>(entity);
        auto& transformComponent    = gCoordinator.getComponent<TransformComponent>(entity);
        auto& renderComponent       = gCoordinator.getComponent<RenderComponent>(entity);

        const b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;
        const auto position = body->GetPosition();

        transformComponent.position = {convertMetersToPixel(position.x), convertMetersToPixel(position.y)};
        renderComponent.sprite.setPosition(position.x, position.y);
        renderComponent.dirty = true;
    }
}

void CollisionSystem::deleteBody(const Entity entity) const
{
    if(gCoordinator.hasComponent<ColliderComponent>(entity))
    {
        auto& cc = gCoordinator.getComponent<ColliderComponent>(entity);
        if (cc.body != nullptr)
            Physics::getWorld()->DestroyBody(cc.body);
        cc.body = nullptr;
        cc.body = {};
    }
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
