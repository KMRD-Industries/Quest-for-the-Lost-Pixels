#include "SpawnerSystem.h"
#include <regex>
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "EquipmentComponent.h"
#include "Helpers.h"
#include "MultiplayerComponent.h"
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "SpawnerComponent.h"
#include "SynchronisedEvent.h"
#include "TextTagComponent.h"
#include "TextureSystem.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

constexpr float SPAWN_RATE = 3600.f;

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
                                   const TransformComponent& spawnerTransformComponent) const
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

    transformComponent.position.x -= colliderComponent.collision.x * configSingleton.GetConfig().gameScale;
    transformComponent.position.y -= colliderComponent.collision.y * configSingleton.GetConfig().gameScale;

    gCoordinator.addComponents(newMonsterEntity, enemyConfig.textureData, transformComponent, RenderComponent{},
                               AnimationComponent{}, EnemyComponent{}, ColliderComponent{enemyConfig.collisionData},
                               CharacterComponent{.hp = enemyConfig.hp});

    const Entity newEventEntity = gCoordinator.createEntity();

    auto newEvent = CreateBodyWithCollisionEvent(
        newMonsterEntity, "Enemy",
        [&, newMonsterEntity, enemyConfig](const GameType::CollisionData& collisionData)
        {
            if (!std::regex_match(collisionData.tag, config::playerRegexTag)) return;

            if (!gCoordinator.hasComponent<CharacterComponent>(collisionData.entity) ||
                gCoordinator.hasComponent<MultiplayerComponent>(collisionData.entity))
                return;
            auto& playerCharacterComponent{gCoordinator.getComponent<CharacterComponent>(collisionData.entity)};
            playerCharacterComponent.attacked = true;

            playerCharacterComponent.hp -= enemyConfig.damage;
            if (playerCharacterComponent.hp <= 0)
            {
                ResourceManager::getInstance().setCurrentShader(video::FragmentShader::DEATH);
                gCoordinator.removeComponent<RenderComponent>(collisionData.entity);
                gCoordinator.removeComponent<PlayerComponent>(collisionData.entity);
                gCoordinator.getComponent<ColliderComponent>(collisionData.entity).toRemoveCollider = true;

                const Entity eventEntity = gCoordinator.createEntity();
                gCoordinator.addComponent(eventEntity,
                                          SynchronisedEvent{.variant = SynchronisedEvent::Variant::PLAYER_KILLED,
                                                            .entity = collisionData.entity});
            }

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{.color = sf::Color::Magenta});
            gCoordinator.addComponent(
                tag, TransformComponent{gCoordinator.getComponent<TransformComponent>(collisionData.entity)});

            if (!config::applyKnockback) return;

            auto& playerCollisionComponent{gCoordinator.getComponent<ColliderComponent>(collisionData.entity)};
            auto& myCollisionComponent{gCoordinator.getComponent<ColliderComponent>(newMonsterEntity)};

            b2Vec2 knockbackDirection{playerCollisionComponent.body->GetPosition() -
                                      myCollisionComponent.body->GetPosition()};
            knockbackDirection.Normalize();

            const auto knockbackForce{configSingleton.GetConfig().defaultEnemyKnockbackForce * knockbackDirection};
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

void SpawnerSystem::spawnEnemies()
{
    for (const auto entity : m_entities)
    {
        auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
        const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        processSpawner(spawnerComponent, spawnerTransformComponent);
    }
    cleanUpUnnecessarySpawners();
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
