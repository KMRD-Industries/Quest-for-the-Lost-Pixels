#include "ChestSpawnerSystem.h"

#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "ItemAnimationComponent.h"
#include "ItemComponent.h"
#include "LootComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void ChestSpawnerSystem::spawnChest()
{
    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<LootComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawn(spawnerTransformComponent);
    }
}


void ChestSpawnerSystem::clearSpawners() const
{
}

void ChestSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent)
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
    gCoordinator.addComponent(chest, ChestComponent{});
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

void ChestSpawnerSystem::handleChestCollision(const Entity chest, const TransformComponent& spawnerTransformComponent,
                                              const GameType::CollisionData& collisionData)
{
    const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
    if (!isCollidingWithPlayer)
        return;

    gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
}

void ChestSpawnerSystem::spawnPotion(const TransformComponent& spawnerTransformComponent)
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
    gCoordinator.addComponent(potion, ItemComponent{itemConfig.name, itemConfig.value, itemConfig.behaviour,
                                                    itemConfig.textureData});
    gCoordinator.addComponent(potion, ItemAnimationComponent{.animationDuration = 1,
                                                             .startingPositionY = spawnerTransformComponent.position.
                                                             y});
}