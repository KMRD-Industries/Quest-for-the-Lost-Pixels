#include "SpawnerSystem.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr int SPAWN_RATE = 3600;

void SpawnerSystem::update()
{
    incrementSpawnTimer();

    for (const auto& entity : m_entities)
    {
        processSpawner(entity);
    }
}

void SpawnerSystem::incrementSpawnTimer() { spawnTime = (spawnTime + 1) % SPAWN_RATE; }

void SpawnerSystem::processSpawner(Entity entity)
{
    auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
    auto spawnCooldown = spawner.spawnCooldown;

    if (!isReadyToSpawn(spawnCooldown))
    {
        return;
    }

    spawnEnemy(entity);
}

bool SpawnerSystem::isReadyToSpawn(int cooldown) const { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(Entity entity)
{
    auto& spawnerTransform = gCoordinator.getComponent<TransformComponent>(entity);

    Entity new_monster = gCoordinator.getRegisterSystem<EnemySystem>()->getFirstUnused();

    auto& enemyTileComponent = gCoordinator.getComponent<TileComponent>(new_monster);
    auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(new_monster);
    auto& enemyAnimationComponent = gCoordinator.getComponent<AnimationComponent>(new_monster);

    enemyAnimationComponent.frames = gCoordinator.getRegisterSystem<TextureSystem>()->getAnimations(
        enemyTileComponent.tileset, enemyTileComponent.id);
    enemyAnimationComponent.it = enemyAnimationComponent.frames.begin();

    enemyTileComponent = {19, "AnimSlimes", 4};
    enemyTransformComponent = TransformComponent(spawnerTransform.position, 0., sf::Vector2f(1., 1.));
};
