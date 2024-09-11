#include "SpawnerSystem.h"
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "RenderComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr int SPAWN_RATE = 3600;

void SpawnerSystem::update()
{
    spawnTime = (spawnTime + 1) % SPAWN_RATE;

    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawner(spawnerComponent, spawnerTransformComponent);
    }

    cleanUpUnnecessarySpawners();
}

void SpawnerSystem::processSpawner(const Entity entity) const
{
    auto& [spawnCooldown, loopSpawn, noSpawns] = gCoordinator.getComponent<SpawnerComponent>(entity);

    if (isReadyToSpawn(static_cast<int>(spawnCooldown)) && !(!loopSpawn && noSpawns >= 1))
    {
        spawnEnemy(entity);
        noSpawns++;
    }
}

bool SpawnerSystem::isReadyToSpawn(const int cooldown) const { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(const TransformComponent& spawnerTransformComponent)
{
    const auto& spawnerTransform = gCoordinator.getComponent<TransformComponent>(entity);
    const Entity newMonsterEntity = gCoordinator.createEntity();
    const Entity newMonsterEntity = gCoordinator.createEntity();

    gCoordinator.addComponent(newMonsterEntity, RenderComponent{});
    gCoordinator.addComponent(newMonsterEntity, TileComponent{});
    gCoordinator.addComponent(newMonsterEntity, TransformComponent{});
    gCoordinator.addComponent(newMonsterEntity, AnimationComponent{});
    gCoordinator.addComponent(newMonsterEntity, ColliderComponent{});
    gCoordinator.addComponent(newMonsterEntity, EnemyComponent{});
    gCoordinator.addComponent(newMonsterEntity, CharacterComponent{.hp = 10.f});
    const TileComponent tileComponent{18, "AnimSlimes", 4};
    const ColliderComponent colliderComponent{
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id)};

    auto& enemyTileComponent = gCoordinator.getComponent<TileComponent>(newMonsterEntity);
    auto& enemyTransformComponent = gCoordinator.getComponent<TransformComponent>(newMonsterEntity);
    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(newMonsterEntity);
    auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(newMonsterEntity);
    gCoordinator.addComponent(newMonsterEntity, tileComponent);
    gCoordinator.addComponent(newMonsterEntity, spawnerTransformComponent);
    gCoordinator.addComponent(newMonsterEntity, colliderComponent);
    gCoordinator.addComponent(newMonsterEntity, RenderComponent{});
    gCoordinator.addComponent(newMonsterEntity, AnimationComponent{});
    gCoordinator.addComponent(newMonsterEntity, EnemyComponent{});
    gCoordinator.addComponent(newMonsterEntity, CharacterComponent{.hp = config::defaultCharacterHP});

    enemyTransformComponent = TransformComponent(spawnerTransform.position, 0., sf::Vector2f(1., 1.), {0.f, 0.f});
    characterComponent.hp = 10.f;
    enemyTileComponent = {18, "AnimSlimes", 4};
    colliderComponent.collision = gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(
        enemyTileComponent.tileset, enemyTileComponent.id);

    gCoordinator.getRegisterSystem<CollisionSystem>()->createBody(
        newMonsterEntity, "SecondPlayer", {colliderComponent.collision.width, colliderComponent.collision.height},
        [&](const GameType::CollisionData& entityT) {}, [&](const GameType::CollisionData& entityT) {}, false, false,
    CollisionSystem::createBody(
        newMonsterEntity, "SecondPlayer", {colliderComponent.collision.width, colliderComponent.collision.height},
        [&](const GameType::CollisionData&) {}, [&](const GameType::CollisionData&) {}, false, false,
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


void SpawnerSystem::cleanUpUnnecessarySpawners() const
{
    std::unordered_set<Entity> entityToKill{};

    for (const auto entity : m_entities)
    {
        if (const auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
            !spawner.loopSpawn && spawner.noSpawns >= 1)
        {
            entityToKill.insert(entity);
        }
    }
    for (const auto entity : entityToKill) gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}
