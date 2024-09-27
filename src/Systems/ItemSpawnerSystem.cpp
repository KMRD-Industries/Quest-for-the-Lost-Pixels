#include "ItemSpawnerSystem.h"

#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

struct CharacterComponent;
struct ColliderComponent;

ItemSpawnerSystem::ItemSpawnerSystem() { init(); }

void ItemSpawnerSystem::init() {}

void ItemSpawnerSystem::updateAnimation(const float deltaTime)
{
    for (const auto entity : m_entities)
    {
        auto& animationComponent = gCoordinator.getComponent<ItemAnimationComponent>(entity);
        const auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (animationComponent.destroy) continue;
        if (!animationComponent.shouldAnimate)
        {
            const auto& itemBehaviour = gCoordinator.getComponent<ItemComponent>(entity).behaviour;
            auto& itemValue = gCoordinator.getComponent<ItemComponent>(entity).value;

            const Entity newItemEntity = gCoordinator.createEntity();

            const auto newEvent = CreateBodyWithCollisionEvent(
                entity, "Item", [this, entity, itemBehaviour, itemValue](const GameType::CollisionData& collisionData)
                { handlePotionCollision(entity, collisionData, itemBehaviour, itemValue); },
                [&](const GameType::CollisionData&) {}, false, false);

            gCoordinator.addComponent(newItemEntity, newEvent);

            animationComponent.destroy = true;
        }
        else
        {
            animationComponent.currentAnimationTime += deltaTime * 4;
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

            const float newPositionY =
                animationComponent.currentAnimationTime * (animationComponent.currentAnimationTime - 2.f) * 100;
            if (newPositionY >= 0) animationComponent.shouldAnimate = false;

            transformComponent.position.y = animationComponent.startingPositionY + newPositionY;
            transformComponent.position.x += 1.f;
        }
    }
}

void ItemSpawnerSystem::deleteItems()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        if (gCoordinator.hasComponent<ColliderComponent>(entity))
        {
            gCoordinator.getComponent<ColliderComponent>(entity).toDestroy = true;
        }
        else
        {
            entityToRemove.push_back(entity);
        }
    }

    for (const auto entity : entityToRemove)
    {
        gCoordinator.removeComponent<ItemComponent>(entity);
    }
}

void ItemSpawnerSystem::handlePotionCollision(const Entity potion, const GameType::CollisionData& collisionData,
                                              const Items::Behaviours behaviour, const float value)
{
    if (gCoordinator.hasComponent<WeaponComponent>(potion)) return;

    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer) return;

    gCoordinator.getComponent<CharacterComponent>(potion).hp = -1;
    switch (behaviour)
    {
    case Items::Behaviours::HEAL:
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).hp += value;
        break;
    case Items::Behaviours::DMGUP:
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).damage += value;
        break;
    default:
        break;
    }
}