#include "SpawnerSystem.h"
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr int SPAWN_RATE = 3600;

void SpawnerSystem::update()
{
    incrementSpawnTimer();

    for (const auto entity : m_entities)
    {
        processSpawner(entity);
    }
}

void SpawnerSystem::incrementSpawnTimer() { spawnTime = (spawnTime + 1) % SPAWN_RATE; }

void SpawnerSystem::processSpawner(const Entity entity)
{
    auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
    auto spawnCooldown = spawner.spawnCooldown;

    if (!spawner.loopSpawn && spawner.noSpawns >= 1)
    {
        return;
    }

    if (!isReadyToSpawn(spawnCooldown))
    {
        return;
    }

    spawnEnemy(entity);
    spawner.noSpawns++;
}

bool SpawnerSystem::isReadyToSpawn(const int cooldown) const { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(const Entity entity)
{
    auto& spawnerTransform = gCoordinator.getComponent<TransformComponent>(entity);

    Entity new_monster = gCoordinator.getRegisterSystem<EnemySystem>()->getFirstUnused();

    auto& enemyTileComponent = gCoordinator.getComponent<TileComponent>(new_monster);
    auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(new_monster);
    auto& enemyAnimationComponent = gCoordinator.getComponent<AnimationComponent>(new_monster);
    auto& enemyCollisionComponent = gCoordinator.getComponent<ColliderComponent>(new_monster);
    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(new_monster);

    characterComponent.hp = 10.f;
    enemyTileComponent = {19, "AnimSlimes", 4};

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        new_monster, "SecondPlayer", {16.f, 16.f}, [&](const GameType::CollisionData& entityT) {},
        [&](const GameType::CollisionData& entityT) {}, false, false);

    enemyTransformComponent = TransformComponent(spawnerTransform.position, 0., sf::Vector2f(1., 1.));
}

void SpawnerSystem::clearSpawners() const
{
    std::deque<Entity> entityToRemove;
    for (const auto& entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<SpawnerComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}
