#include "LootSpawnerSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "LootComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void LootSpawnerSystem::spawnChest() const
{
    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<LootComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawn(spawnerTransformComponent);
    }
}

void LootSpawnerSystem::clearSpawners() const
{
}

void LootSpawnerSystem::processSpawn(const TransformComponent& spawnerTransformComponent) const
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
        [chest](const GameType::CollisionData& collisionData)
        {
            const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
            if (!isCollidingWithPlayer)
                return;

            gCoordinator.getComponent<CharacterComponent>(chest).hp = -1;
        }, [&](const GameType::CollisionData&)
        {
        }, true, true,
        {colliderComponent.collision.x, colliderComponent.collision.y});
}