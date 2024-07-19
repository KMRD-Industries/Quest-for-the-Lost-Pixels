#include "SpawnerSystem.h"
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "RenderComponent.h"
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

void SpawnerSystem::processSpawner(const Entity entity) const
{
    auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
    const auto spawnCooldown = spawner.spawnCooldown;

    if (!spawner.loopSpawn && spawner.noSpawns >= 1)
    {
        return;
    }

    if (!isReadyToSpawn(static_cast<int>(spawnCooldown)))
    {
        return;
    }

    spawnEnemy(entity);
    spawner.noSpawns++;
}

bool SpawnerSystem::isReadyToSpawn(const int cooldown) const { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(const Entity entity)
{
    const auto& spawnerTransform = gCoordinator.getComponent<TransformComponent>(entity);

    const Entity new_monster = gCoordinator.createEntity();

    gCoordinator.addComponent(new_monster, RenderComponent{});
    gCoordinator.addComponent(new_monster, TileComponent{});
    gCoordinator.addComponent(new_monster, TransformComponent{});
    gCoordinator.addComponent(new_monster, AnimationComponent{});
    gCoordinator.addComponent(new_monster, ColliderComponent{});
    gCoordinator.addComponent(new_monster, EnemyComponent{});
    gCoordinator.addComponent(new_monster, CharacterComponent{.hp = 10.f});

    auto& enemyTileComponent = gCoordinator.getComponent<TileComponent>(new_monster);
    auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(new_monster);
    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(new_monster);
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(new_monster);

    enemyTransformComponent = TransformComponent(spawnerTransform.position, 0., sf::Vector2f(1., 1.), {0.f, 0.f});
    characterComponent.hp = 10.f;
    enemyTileComponent = {18, "AnimSlimes", 4};
    colliderComponent.collision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(
        enemyTileComponent.tileset, enemyTileComponent.id);

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        new_monster, "SecondPlayer", {colliderComponent.collision.width, colliderComponent.collision.height},
        [&](const GameType::CollisionData& entityT) {}, [&](const GameType::CollisionData& entityT) {}, false, false,
        {colliderComponent.collision.x, colliderComponent.collision.y});
}

void SpawnerSystem::clearSpawners() const
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
    {
        entityToRemove.push_back(entity);
    }

    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<SpawnerComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}
