#include "CollisionSystem.h"

#include "AnimationComponent.h"
#include "BodyArmourComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "HelmetComponent.h"
#include "ItemComponent.h"
#include "MultiplayerSystem.h"
#include "Physics.h"
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
        if (!gCoordinator.hasComponent<ColliderComponent>(bodyAData->entity)) return;
        if (!gCoordinator.hasComponent<ColliderComponent>(bodyBData->entity)) return;
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entity);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entity);

        if (!colliderComponentA.onCollisionEnter && !colliderComponentB.onCollisionEnter) return;

        if (colliderComponentA.onCollisionEnter)
            (*colliderComponentA.onCollisionEnter)({bodyBData->entity, bodyBData->tag});

        if (colliderComponentB.onCollisionEnter)
            (*colliderComponentB.onCollisionEnter)({bodyAData->entity, bodyAData->tag});
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
        // TODO: Fix weapon collision in other task
        if (!gCoordinator.hasComponent<ColliderComponent>(bodyAData->entity)) return;
        if (!gCoordinator.hasComponent<ColliderComponent>(bodyBData->entity)) return;
        const auto& colliderComponentA = gCoordinator.getComponent<ColliderComponent>(bodyAData->entity);
        const auto& colliderComponentB = gCoordinator.getComponent<ColliderComponent>(bodyBData->entity);

        if (!colliderComponentA.onCollisionOut && !colliderComponentB.onCollisionOut) return;

        if (colliderComponentA.onCollisionOut)
            (*colliderComponentA.onCollisionOut)({bodyBData->entity, bodyBData->tag});

        if (colliderComponentB.onCollisionOut)
            (*colliderComponentB.onCollisionOut)({bodyAData->entity, bodyAData->tag});
    }
}

CollisionSystem::CollisionSystem() { Physics::getWorld()->SetContactListener(&m_myContactListenerInstance); }

void CollisionSystem::createMapCollision()
{
    auto createCollisionBody =
        [this](const Entity entity, const std::string& type, const bool isStatic, const bool useTexture)
    {
        const Entity newMapCollisionEntity = gCoordinator.createEntity();

        const auto createBodyEvent = CreateBodyWithCollisionEvent{
            .entity = entity,
            .tag = type,
            .isStatic = isStatic,
            .useTextureSize = useTexture,
        };

        gCoordinator.addComponent(newMapCollisionEntity, createBodyEvent);
    };

    for (const auto entity : m_entities)
    {
        const auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (tileComponent.id < 0 || tileComponent.tileSet.empty() ||
            gCoordinator.hasComponent<PlayerComponent>(entity) || gCoordinator.hasComponent<ItemComponent>(entity))
            continue;

        if (tileComponent.tileSet == "SpecialBlocks")
        {
            if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::STATICWALLCOLLIDER))
                createCollisionBody(entity, "Wall", true, true);

            else if (tileComponent.id == static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER))
                createCollisionBody(entity, "Door", true, false);
        }
        else
        {
            if (colliderComponent.collision.width > 0 && colliderComponent.collision.height > 0)
                createCollisionBody(entity, "Wall", true, false);
        }
    }
}

void CollisionSystem::update(const float& deltaTime)
{
    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * 1000)
    {
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * 1000;
        performFixedUpdate();
    }
}

void CollisionSystem::performFixedUpdate() const
{
    for (const auto entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        b2Body* body = colliderComponent.body;
        if (body == nullptr) continue;

        if (gCoordinator.hasComponent<PlayerComponent>(entity))
        {
            if (transformComponent.velocity != b2Vec2{0.f, 0.f})
            {
                gCoordinator.getComponent<AnimationComponent>(entity).currentState =
                    AnimationStateMachine::AnimationState::Idle;
            }

            if (transformComponent.velocity == b2Vec2{0.f, 0.f} && body->GetLinearVelocity() == b2Vec2{0.f, 0.f})
            {
                gCoordinator.getComponent<AnimationComponent>(entity).currentState =
                    AnimationStateMachine::AnimationState::Running;
            }
        }

        if (!transformComponent.velocity.IsValid()) continue;

        if (colliderComponent.tag == "Item")
            body->ApplyForceToCenter({transformComponent.velocity.x, transformComponent.velocity.y}, true);
        else
        {
            body->SetLinearVelocity({convertPixelsToMeters(transformComponent.velocity.x),
                                     convertPixelsToMeters(transformComponent.velocity.y)});
        }
        if (colliderComponent.trigger)
        {
            // TODO: Normal space
            const auto center = glm::vec2{transformComponent.position.x + colliderComponent.weaponPlacement.x,
                                          transformComponent.position.y + colliderComponent.weaponPlacement.y};

            body->SetTransform({convertPixelsToMeters(center.x), convertPixelsToMeters(center.y)},
                               transformComponent.rotation * M_PI / 180);
        }
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
        const auto position = body->GetPosition();

        transformComponent.position = {convertMetersToPixel(position.x), convertMetersToPixel(position.y)};
        transformComponent.rotation = body->GetAngle() * 180.f / M_PI;
        renderComponent.sprite.setPosition(position.x, position.y);
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

    for (auto& entity : entityToKill)
    {
        gCoordinator.destroyEntity(entity);
    }
    entityToKill.clear();
}