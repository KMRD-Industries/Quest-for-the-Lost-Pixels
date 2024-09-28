#include "SpawnerSystem.h"

#include <regex>

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "RenderComponent.h"
#include "SpawnerComponent.h"
#include "TextureSystem.h"
#include "TileComponent.h"
#include "TransformComponent.h"

constexpr float SPAWN_RATE = 3600.f;

SpawnerSystem::SpawnerSystem() { init(); }

void SpawnerSystem::init() { m_spawnTime = {}; }

void SpawnerSystem::update(const float timeStamp)
{
    m_spawnTime += timeStamp * 1000.f;

    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        processSpawner(spawnerComponent, spawnerTransformComponent);
    }

    if (m_spawnTime >= SPAWN_RATE) m_spawnTime -= SPAWN_RATE;

    cleanUpUnnecessarySpawners();
}

void SpawnerSystem::processSpawner(SpawnerComponent& spawnerComponent,
                                   const TransformComponent& spawnerTransformComponent)
{
    // Check if the spawner is ready to spawn the enemy.
    if (m_spawnTime < SPAWN_RATE) return;

    // Spawn the enemy and increment the spawn count.
    spawnerComponent.noSpawns++;
    spawnEnemy(spawnerTransformComponent, spawnerComponent.enemyType);
}

void SpawnerSystem::spawnEnemy(const TransformComponent& spawnerTransformComponent,
                               const Enemies::EnemyType enemyType) const
{
    const auto enemyConfig = getRandomEnemyData(enemyType);
    const Entity newMonsterEntity = gCoordinator.createEntity();

    const ColliderComponent colliderComponent{enemyConfig.collisionData};
    TransformComponent transformComponent{spawnerTransformComponent};

    transformComponent.position.x -= colliderComponent.collision.x * config::gameScale;
    transformComponent.position.y -= colliderComponent.collision.y * config::gameScale;

    gCoordinator.addComponents(newMonsterEntity, enemyConfig.textureData, transformComponent, RenderComponent{},
                               AnimationComponent{}, EnemyComponent{}, ColliderComponent{enemyConfig.collisionData},
                               CharacterComponent{.hp = enemyConfig.hp});

    const Entity newEventEntity = gCoordinator.createEntity();

    auto newEvent = CreateBodyWithCollisionEvent(
        newMonsterEntity, "Enemy",
        [&, newMonsterEntity, enemyConfig](const GameType::CollisionData& collisionData)
        {
            if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;

            auto& playerCharacterComponent{gCoordinator.getComponent<CharacterComponent>(collisionData.entityID)};

            playerCharacterComponent.attacked = true;
            playerCharacterComponent.hp -= enemyConfig.damage;

            if (!config::applyKnockback) return;

            auto& playerCollisionComponent{gCoordinator.getComponent<ColliderComponent>(collisionData.entityID)};
            auto& myCollisionComponent{gCoordinator.getComponent<ColliderComponent>(newMonsterEntity)};

            b2Vec2 knockbackDirection{playerCollisionComponent.body->GetPosition() -
                                      myCollisionComponent.body->GetPosition()};
            knockbackDirection.Normalize();

            const auto knockbackForce{config::defaultEnemyKnockbackForce * knockbackDirection};
            playerCollisionComponent.body->ApplyLinearImpulseToCenter(knockbackForce, true);
        },
        [&](const GameType::CollisionData&) {}, false, false);

    gCoordinator.addComponent(newEventEntity, newEvent);
}

void SpawnerSystem::clearSpawners()
{
    std::deque<Entity> entityToRemove;

    for (const auto entity : m_entities) entityToRemove.push_back(entity);

    while (!entityToRemove.empty())
    {
        gCoordinator.removeComponent<SpawnerComponent>(entityToRemove.front());
        entityToRemove.pop_front();
    }
}

void SpawnerSystem::cleanUpUnnecessarySpawners()
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
