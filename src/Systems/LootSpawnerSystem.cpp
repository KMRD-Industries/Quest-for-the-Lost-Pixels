#include "LootSpawnerSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "LootComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void LootSpawnerSystem::spawnChest()
{
    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<LootComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawn(spawnerTransformComponent);
    }
}

void LootSpawnerSystem::updateAnimation(const float deltaTime)
{
    for (AnimationData& animationData : m_itemsToAnimate)
    {
        if (animationData.destroy)
            continue;
        if (!animationData.shouldAnimate)
        {
            ColliderComponent& colliderComponent = gCoordinator.getComponent<ColliderComponent>(
                animationData.entityToAnimate);
            CollisionSystem::createBody(
                animationData.entityToAnimate, "Item", {colliderComponent.collision.width,
                                                        colliderComponent.collision.height},
                [this, animationData](const GameType::CollisionData& collisionData)
                {
                    handlePotionCollision(animationData.entityToAnimate, collisionData);
                }, [&](const GameType::CollisionData&)
                {
                }, false, true,
                {colliderComponent.collision.x, colliderComponent.collision.y});
            animationData.destroy = true;
        }
        else
        {
            animationData.currentAnimationTime += deltaTime;
            TransformComponent& transformComponent = gCoordinator.getComponent<TransformComponent>(
                animationData.entityToAnimate);

            const float newPositionY = animationData.currentAnimationTime * (
                animationData.currentAnimationTime - 2.f) * 100;
            if (newPositionY >= 0)
                animationData.shouldAnimate = false;
            transformComponent.position.y = animationData.startingPositionY + newPositionY;

            transformComponent.position.x += 1.f;
        }
    }
}

void LootSpawnerSystem::clearSpawners() const
{
}

void LootSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent)
{
    const Entity chest = gCoordinator.createEntity();
    const TileComponent tileComponent{819, "Items", 4};
    const ColliderComponent colliderComponent{
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id)};

    gCoordinator.addComponent(chest, tileComponent);
    gCoordinator.addComponent(chest, spawnerTransformComponent);
    gCoordinator.addComponent(chest, colliderComponent);
    gCoordinator.addComponent(chest, RenderComponent{});
    gCoordinator.addComponent(chest, AnimationComponent{});
    gCoordinator.addComponent(chest, CharacterComponent{.hp = 100});

    CollisionSystem::createBody(
        chest, "Chest", {colliderComponent.collision.width, colliderComponent.collision.height},
        [this, chest, spawnerTransformComponent](const GameType::CollisionData& collisionData)
        {
            handleChestCollision(chest, spawnerTransformComponent, collisionData);
        }, [this, spawnerTransformComponent](const GameType::CollisionData&)
        {
            spawnPotion(spawnerTransformComponent);
        }, true, true,
        {colliderComponent.collision.x, colliderComponent.collision.y});
}

void LootSpawnerSystem::handleChestCollision(const Entity chest, const TransformComponent& spawnerTransformComponent,
                                             const GameType::CollisionData& collisionData)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer)
        return;

    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void LootSpawnerSystem::spawnPotion(const TransformComponent& spawnerTransformComponent)
{
    const Entity potion = gCoordinator.createEntity();
    const config::ItemConfig itemConfig = getRandomItemData();
    const TileComponent tileComponent{itemConfig.textureData};
    const ColliderComponent colliderComponent{
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id)};

    gCoordinator.addComponent(potion, tileComponent);
    gCoordinator.addComponent(potion, spawnerTransformComponent);
    gCoordinator.addComponent(potion, colliderComponent);
    gCoordinator.addComponent(potion, RenderComponent{});
    gCoordinator.addComponent(potion, AnimationComponent{});
    gCoordinator.addComponent(potion, CharacterComponent{.hp = 100});

    m_itemsToAnimate.push_back({.animationDuration = 1, .entityToAnimate = potion,
                                .startingPositionY = spawnerTransformComponent.position.y});
}

void LootSpawnerSystem::handlePotionCollision(const Entity potion, const GameType::CollisionData& collisionData)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer)
        return;

    gCoordinator.getComponent<CharacterComponent>(potion).hp = -1;
    gCoordinator.getComponent<CharacterComponent>(collisionData.entityID).hp += 50;
}