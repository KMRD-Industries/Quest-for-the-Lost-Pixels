#include "LootSpawnerSystem.h"

#include <TextureSystem.h>

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

    /*
    CollisionSystem::createBody(
        chest, "Chest", {colliderComponent.collision.width, colliderComponent.collision.height},
        [&](const GameType::CollisionData& collisionData)
        {
            const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
            if (!isCollidingWithPlayer)
                return;

            std::cout << "collisionHappend\n";
        }, [&](const GameType::CollisionData&)
        {
        }, false, false,
        {colliderComponent.collision.x, colliderComponent.collision.y});*/
}