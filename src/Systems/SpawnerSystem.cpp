//
// Created by dominiq on 6/1/24.
//

#include "SpawnerSystem.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr int SPAWN_RATE = 3600;

void SpawnerSystem::update()
{
    updateSpawnTime();

    for (const auto& entity : m_entities)
    {
        updateSpawner(entity);
    }
}

void SpawnerSystem::updateSpawnTime() { spawnTime = (spawnTime + 1) % SPAWN_RATE; }

void SpawnerSystem::updateSpawner(Entity entity)
{
    auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
    auto spawnCooldown = spawner.spawnCooldown;

    if (!isTimeForNextEnemy(spawnCooldown))
    {
        return;
    }

    spawnEnemy(entity);
}

bool SpawnerSystem::isTimeForNextEnemy(int cooldown) { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(Entity entity)
{
    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

    Entity new_monster = gCoordinator.getRegisterSystem<EnemySystem>()->getFirstUnused();

    auto& tileComponent = gCoordinator.getComponent<TileComponent>(new_monster);
    auto& monsterTransformComponent = gCoordinator.getComponent<TransformComponent>(new_monster);
    auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(new_monster);

    animationComponent.frames =
        gCoordinator.getRegisterSystem<TextureSystem>()->getAnimations(tileComponent.tileset, tileComponent.id);
    animationComponent.it = animationComponent.frames.begin();

    tileComponent = {19, "AnimSlimes", 4};
    monsterTransformComponent = TransformComponent(transformComponent.position, 0.f, sf::Vector2f(1.f, 1.f));
};
