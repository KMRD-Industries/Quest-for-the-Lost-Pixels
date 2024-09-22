#include "ItemSpawnerSystem.h"

#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "TransformComponent.h"

struct CharacterComponent;
struct ColliderComponent;

void ItemSpawnerSystem::updateAnimation(const float deltaTime)
{
    for (const auto entity : m_entities)
    {
        ItemAnimationComponent& animationComponent = gCoordinator.getComponent<ItemAnimationComponent>(entity);
        if (animationComponent.destroy)
            continue;
        if (!animationComponent.shouldAnimate)
        {
            const auto itemBehaviour = gCoordinator.getComponent<ItemComponent>(entity).behaviour;
            const auto itemValue = gCoordinator.getComponent<ItemComponent>(entity).value;
            ColliderComponent& colliderComponent = gCoordinator.getComponent<ColliderComponent>(
                entity);
            CollisionSystem::createBody(
                entity, "Item", {colliderComponent.collision.width,
                                 colliderComponent.collision.height},
                [this, entity,itemBehaviour,itemValue](const GameType::CollisionData& collisionData)
                {
                    handlePotionCollision(entity, collisionData, itemBehaviour, itemValue);
                }, [&](const GameType::CollisionData&)
                {
                }, false, true,
                {colliderComponent.collision.x, colliderComponent.collision.y});
            animationComponent.destroy = true;
        }
        else
        {
            animationComponent.currentAnimationTime += deltaTime;
            TransformComponent& transformComponent = gCoordinator.getComponent<TransformComponent>(
                entity);

            const float newPositionY = animationComponent.currentAnimationTime * (
                animationComponent.currentAnimationTime - 2.f) * 100;
            if (newPositionY >= 0)
                animationComponent.shouldAnimate = false;
            transformComponent.position.y = animationComponent.startingPositionY + newPositionY;

            transformComponent.position.x += 1.f;
        }
    }
}

void ItemSpawnerSystem::deleteItems()
{
    for (const auto entity : m_entities)
        gCoordinator.getComponent<CharacterComponent>(entity).hp = -1;
}

void ItemSpawnerSystem::handlePotionCollision(const Entity potion, const GameType::CollisionData& collisionData,
                                              const Items::Behaviours behaviour, const float value)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer)
        return;

    gCoordinator.getComponent<CharacterComponent>(potion).hp = -1;
    switch (behaviour)
    {
    case (Items::Behaviours::HEAL):
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).hp += value;
        break;
    case (Items::Behaviours::DMGUP):
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).damage += value;
        break;
    }
}