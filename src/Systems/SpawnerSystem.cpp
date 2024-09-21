#include "SpawnerSystem.h"

#include <regex>

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr int SPAWN_RATE = 3600;

void SpawnerSystem::update()
{
    incrementSpawnTimer();

    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawner(spawnerComponent, spawnerTransformComponent);
    }

    cleanUpUnnecessarySpawners();
}

void SpawnerSystem::incrementSpawnTimer() { spawnTime = (spawnTime + 1) % SPAWN_RATE; }

void SpawnerSystem::processSpawner(SpawnerComponent& spawnerComponent,
                                   const TransformComponent& spawnerTransformComponent) const
{
    if (!spawnerComponent.loopSpawn && spawnerComponent.noSpawns >= 1)
        return;

    // Check if the spawner is ready to spawn the enemy.
    if (!isReadyToSpawn(static_cast<int>(spawnerComponent.spawnCooldown)))
        return;

    // Spawn the enemy and increment the spawn count.
    spawnerComponent.noSpawns++;
    spawnEnemy(spawnerTransformComponent, spawnerComponent.enemyType);
}

void SpawnerSystem::cleanUpUnnecessarySpawners() const
{
    std::unordered_set<Entity> entityToKill{};

    for (const auto entity : m_entities)
        if (const auto& spawner = gCoordinator.getComponent<SpawnerComponent>(entity);
            !spawner.loopSpawn && spawner.noSpawns >= 1)
            entityToKill.insert(entity);
    for (const auto entity : entityToKill)
        gCoordinator.destroyEntity(entity);
    entityToKill.clear();
}

bool SpawnerSystem::isReadyToSpawn(const int cooldown) const { return spawnTime % cooldown == 0; }

void SpawnerSystem::spawnEnemy(const TransformComponent& spawnerTransformComponent, const Enemies::EnemyType enemyType)
{
    const auto enemyConfig = getRandomEnemyData(enemyType);


    const Entity newMonsterEntity = gCoordinator.createEntity();
    const TileComponent tileComponent{enemyConfig.textureData};
    const ColliderComponent colliderComponent{
        gCoordinator.getRegisterSystem<TextureSystem>()->getCollision(tileComponent.tileset, tileComponent.id)};

    gCoordinator.addComponent(newMonsterEntity, tileComponent);
    gCoordinator.addComponent(newMonsterEntity, spawnerTransformComponent);
    gCoordinator.addComponent(newMonsterEntity, colliderComponent);
    gCoordinator.addComponent(newMonsterEntity, RenderComponent{});
    gCoordinator.addComponent(newMonsterEntity, AnimationComponent{});
    gCoordinator.addComponent(newMonsterEntity, EnemyComponent{});
    gCoordinator.addComponent(newMonsterEntity, CharacterComponent{.hp = enemyConfig.hp});

    CollisionSystem::createBody(
        newMonsterEntity, "Enemy", {colliderComponent.collision.width, colliderComponent.collision.height},
        [&, newMonsterEntity](const GameType::CollisionData& collisionData)
        {
            const bool isCollidingWithPlayer = std::regex_match(collisionData.tag, config::playerRegexTag);
            if (!isCollidingWithPlayer)
                return;

            auto& playerCharacterComponent{gCoordinator.getComponent<CharacterComponent>(collisionData.entityID)};
            playerCharacterComponent.attacked = true;

            playerCharacterComponent.hp -= enemyConfig.damage;

            if (!config::applyKnockback)
                return;

            auto& playerCollisionComponent{gCoordinator.getComponent<ColliderComponent>(collisionData.entityID)};
            auto& myCollisionComponent{gCoordinator.getComponent<ColliderComponent>(newMonsterEntity)};

            b2Vec2 knockbackDirection{playerCollisionComponent.body->GetPosition() - myCollisionComponent.body->
                GetPosition()};
            knockbackDirection.Normalize();

            const auto knockbackForce{config::defaultEnemyKnockbackForce * knockbackDirection};
            playerCollisionComponent.body->ApplyLinearImpulseToCenter(knockbackForce, true);
        }, [&](const GameType::CollisionData&)
        {
        }, false, false,
        {colliderComponent.collision.x, colliderComponent.collision.y});
}

void SpawnerSystem::clearSpawners() const
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities)
        entityToRemove.push_back(entity);

    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<SpawnerComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}