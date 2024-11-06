#include "SpawnerSystem.h"
#include <regex>
#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EnemyComponent.h"
#include "EnemySystem.h"
#include "Helpers.h"
#include "PlayerComponent.h"
#include "MultiplayerSystem.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "SpawnerComponent.h"
#include "TextTagComponent.h"
#include "TextureSystem.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern PublicConfigSingleton configSingleton;

constexpr float SPAWN_RATE = 3600.f;

void SpawnerSystem::update(const float timeStamp)
{
    // for (const auto entity : m_entities)
    // {
    //     auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
    //     const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    //
    //     processSpawner(spawnerComponent, spawnerTransformComponent);
    // }

    prepareEnemies();
    // TODO przenieś do stałej
    m_spawnTime += timeStamp * 1000.f;

    if (m_spawnTime >= SPAWN_RATE) m_spawnTime -= SPAWN_RATE;

    cleanUpUnnecessarySpawners();
}

void SpawnerSystem::processSpawner(SpawnerComponent& spawnerComponent,
                                   const TransformComponent& spawnerTransformComponent) const
{
    // Check if the spawner is ready to spawn the enemy.
    //TODO idk czy to nie psuje
    if (m_spawnTime < SPAWN_RATE) return;

    // Spawn the enemy and increment the spawn count.
    spawnerComponent.noSpawns++;
    // auto _ = spawnEnemy(spawnerTransformComponent, spawnerComponent.enemyType);
}

Entity SpawnerSystem::spawnEnemy(const comm::Enemy& enemyToSpawn) const
{
    auto positionsVec = sf::Vector2f(enemyToSpawn.x(), enemyToSpawn.y());
    TransformComponent transformComponent{positionsVec};

    const Entity newMonsterEntity = gCoordinator.createEntity();
    const Collision collisionData{
        enemyToSpawn.collisiondata().type(),    enemyToSpawn.collisiondata().xoffset(),
        enemyToSpawn.collisiondata().yoffset(), enemyToSpawn.collisiondata().width(),
        enemyToSpawn.collisiondata().height(),
    };
    const ColliderComponent colliderComponent{collisionData};

    transformComponent.position.x -= colliderComponent.collision.x * configSingleton.GetConfig().gameScale;
    transformComponent.position.y -= colliderComponent.collision.y * configSingleton.GetConfig().gameScale;

    TileComponent tileComponent{enemyToSpawn.texturedata().tileid(), enemyToSpawn.texturedata().tileset(),
                                enemyToSpawn.texturedata().tilelayer()};

    gCoordinator.addComponents(newMonsterEntity, tileComponent, transformComponent, RenderComponent{},
                               AnimationComponent{}, EnemyComponent{}, ColliderComponent{collisionData},
                               CharacterComponent{.hp = static_cast<float>(enemyToSpawn.hp())});

    const Entity newEventEntity = gCoordinator.createEntity();

    auto newEvent = CreateBodyWithCollisionEvent(
        newMonsterEntity, "Enemy",
        [&, newMonsterEntity, enemyToSpawn](const GameType::CollisionData& collision)
        {
            if (!std::regex_match(collision.tag, config::playerRegexTag)) return;

            if (!gCoordinator.hasComponent<CharacterComponent>(collision.entityID)) return;
            auto& playerCharacterComponent{gCoordinator.getComponent<CharacterComponent>(collision.entityID)};
            playerCharacterComponent.attacked = true;

            playerCharacterComponent.hp -= enemyToSpawn.damage();
            if (playerCharacterComponent.hp <= 0)
            {
                ResourceManager::getInstance().setCurrentShader(video::FragmentShader::DEATH);
                gCoordinator.removeComponent<CharacterComponent>(collision.entityID);
                gCoordinator.removeComponent<RenderComponent>(collision.entityID);
                gCoordinator.removeComponent<PlayerComponent>(collision.entityID);
                gCoordinator.getComponent<ColliderComponent>(collision.entityID).toRemoveCollider = true;
            }

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{.color = sf::Color::Magenta});
            gCoordinator.addComponent(
                tag, TransformComponent{gCoordinator.getComponent<TransformComponent>(collision.entityID)});

            if (!config::applyKnockback) return;

            auto& playerCollisionComponent{gCoordinator.getComponent<ColliderComponent>(collision.entityID)};
            auto& myCollisionComponent{gCoordinator.getComponent<ColliderComponent>(newMonsterEntity)};

            b2Vec2 knockbackDirection{playerCollisionComponent.body->GetPosition() -
                                      myCollisionComponent.body->GetPosition()};
            knockbackDirection.Normalize();

            const auto knockbackForce{configSingleton.GetConfig().defaultEnemyKnockbackForce * knockbackDirection};
            playerCollisionComponent.body->ApplyLinearImpulseToCenter(knockbackForce, true);
        },
        [&](const GameType::CollisionData&) {}, false, false);

    gCoordinator.addComponent(newEventEntity, newEvent);
    return newMonsterEntity;
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
    // for (const auto entity : m_entities)
    // {
    //     auto& spawnerComponent = gCoordinator.getComponent<SpawnerComponent>(entity);
    //     const auto& spawnerTransformComponent = gCoordinator.getComponent<TransformComponent>(entity);
    //     processSpawner(spawnerComponent, spawnerTransformComponent);
    // }
    prepareEnemies();
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

std::vector<std::pair<Entity, sf::Vector2<float>>> SpawnerSystem::getSortedSpawnedEnemies() { return m_spawnedEnemies; }

void SpawnerSystem::prepareEnemies()
{
    for (auto enemyEntity : m_entities)
    {
        m_spawnedEnemies.push_back(
            std::make_pair(enemyEntity, gCoordinator.getComponent<TransformComponent>(enemyEntity).position));
    }

    std::sort(m_spawnedEnemies.begin(), m_spawnedEnemies.end(),
              [](const std::pair<Entity, sf::Vector2<float>>& a, const std::pair<Entity, sf::Vector2<float>>& b)
              { return a.second.x > b.second.x; });

    std::shared_ptr<MultiplayerSystem> multiplayer_system = gCoordinator.getRegisterSystem<MultiplayerSystem>();
    multiplayer_system->sendSpawnerPosition(m_spawnedEnemies);
}

void SpawnerSystem::spawnOnDemand(const comm::EnemyPositionsUpdate& enemiesToSpawn) const
{
    for (auto enemyToSpawn : enemiesToSpawn.enemypositions())
    {
        SpawnerComponent spawnerComponent{};
        spawnerComponent.enemyType = Enemies::EnemyType::MELEE;
        const Entity newEnemyEntity = spawnEnemy(enemyToSpawn);
        gCoordinator.mapEntity(enemyToSpawn.id(), newEnemyEntity);
    }
}
