#include "ItemSpawnerSystem.h"

#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "TransformComponent.h"

struct CharacterComponent;
struct ColliderComponent;

ItemSpawnerSystem::ItemSpawnerSystem() { init(); }

void ItemSpawnerSystem::init()
{
    gen.seed(rd()); // Seed the generator
    dis = std::uniform_int_distribution(-5, 5);
}


void ItemSpawnerSystem::updateAnimation(const float deltaTime)
{
    for (const auto entity : m_entities)
    {
        auto& animationComponent = gCoordinator.getComponent<ItemAnimationComponent>(entity);
        const auto colliderComponent = gCoordinator.getComponent<ColliderComponent>(entity);

        if (animationComponent.destroy) continue;
        if (animationComponent.shouldAnimate)
        {
            const auto& itemBehaviour = gCoordinator.getComponent<ItemComponent>(entity).behaviour;
            auto& item = gCoordinator.getComponent<ItemComponent>(entity);
            TransformComponent& transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto& itemValue = gCoordinator.getComponent<ItemComponent>(entity).value;
            item.timestamp = std::chrono::system_clock::now();

            const Entity newItemEntity = gCoordinator.createEntity();

            transform.velocity = {static_cast<float>(dis(gen)), static_cast<float>(dis(gen))};

            const auto newEvent = CreateBodyWithCollisionEvent(
                entity, "Item", [this, entity, itemBehaviour, itemValue](const GameType::CollisionData& collisionData)
                { handlePotionCollision(entity, collisionData, itemBehaviour, itemValue); },
                [&](const GameType::CollisionData&) {}, false, false);

            gCoordinator.addComponent(newItemEntity, newEvent);

            animationComponent.destroy = true;
        }
        else
        {
            animationComponent.currentAnimationTime += deltaTime;
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
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        Physics::getWorld()->DestroyBody(gCoordinator.getComponent<ColliderComponent>(entityToRemove.front()).body);
        gCoordinator.destroyEntity(entityToRemove.front());
        entityToRemove.pop_front();
    }
}

void ItemSpawnerSystem::handlePotionCollision(const Entity potion, const GameType::CollisionData& collisionData,
                                              const Items::Behaviours behaviour, const float value)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer) return;

    const auto& itemComponent = gCoordinator.getComponent<ItemComponent>(potion);
    if (itemComponent.timestamp + std::chrono::seconds(1) > std::chrono::system_clock::now()) return;

    gCoordinator.getComponent<CharacterComponent>(potion).hp = -1;
    switch (behaviour)
    {
    case Items::Behaviours::HEAL:
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).hp += value;
        break;
    case Items::Behaviours::DMGUP:
        gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).damage += value;
        break;
    }
}