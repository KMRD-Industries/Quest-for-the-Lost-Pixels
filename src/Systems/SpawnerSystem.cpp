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
#include "MultiplayerSystem.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "SpawnerComponent.h"
#include "SynchronisedEvent.h"
#include "TextTagComponent.h"
#include "TextureSystem.h"
#include "TransformComponent.h"

extern PublicConfigSingleton configSingleton;

constexpr float SPAWN_RATE = 3600.f;

void SpawnerSystem::update(const float timeStamp)
{
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
    if (m_spawnTime < SPAWN_RATE) return;

    spawnerComponent.noSpawns++;
}

void SpawnerSystem::spawnEnemy(Entity newMonsterEntity, const comm::Enemy& enemyToSpawn) const
{
    auto positionsVec = sf::Vector2f(enemyToSpawn.position_x(), enemyToSpawn.position_y());
    TransformComponent transformComponent{positionsVec};

    const Collision collisionData{
        enemyToSpawn.collision_data().type(),     enemyToSpawn.collision_data().x_offset(),
        enemyToSpawn.collision_data().y_offset(), enemyToSpawn.collision_data().width(),
        enemyToSpawn.collision_data().height(),
    };
    const ColliderComponent colliderComponent{collisionData};

    transformComponent.position.x -= colliderComponent.collision.x * configSingleton.GetConfig().gameScale;
    transformComponent.position.y -= colliderComponent.collision.y * configSingleton.GetConfig().gameScale;

    TileComponent tileComponent{enemyToSpawn.texture_data().tile_id(), enemyToSpawn.texture_data().tile_set(),
                                enemyToSpawn.texture_data().tile_layer()};

    gCoordinator.addComponents(newMonsterEntity, tileComponent, transformComponent, RenderComponent{},
                               AnimationComponent{}, EnemyComponent{}, ColliderComponent{collisionData},
                               CharacterComponent{.hp = static_cast<float>(enemyToSpawn.hp())});

    const Entity newEventEntity = gCoordinator.createEntity();

    auto newEvent = CreateBodyWithCollisionEvent(
        newMonsterEntity, "Enemy",
        [&, newMonsterEntity, enemyToSpawn](const GameType::CollisionData& collision)
        {
            if (!std::regex_match(collision.tag, config::playerRegexTag)) return;

            if (!gCoordinator.hasComponent<CharacterComponent>(collision.entity)) return;
            auto& playerCharacterComponent{gCoordinator.getComponent<CharacterComponent>(collision.entity)};
            playerCharacterComponent.attacked = true;

            playerCharacterComponent.hp -= enemyToSpawn.damage();
            if (playerCharacterComponent.hp <= 0)
            {
                ResourceManager::getInstance().setCurrentShader(video::FragmentShader::DEATH);
                gCoordinator.removeComponent<RenderComponent>(collision.entity);
                gCoordinator.removeComponent<PlayerComponent>(collision.entity);
                gCoordinator.getComponent<ColliderComponent>(collision.entity).toRemoveCollider = true;

                const Entity eventEntity = gCoordinator.createEntity();
                gCoordinator.addComponent(eventEntity,
                                          SynchronisedEvent{.variant = SynchronisedEvent::Variant::PLAYER_KILLED,
                                                            .entity = collision.entity});
            }

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{.color = sf::Color::Magenta});
            gCoordinator.addComponent(
                tag, TransformComponent{gCoordinator.getComponent<TransformComponent>(collision.entity)});

            if (!config::applyKnockback) return;

            auto& playerCollisionComponent{gCoordinator.getComponent<ColliderComponent>(collision.entity)};
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
    for (const auto enemyEntity : m_entities)
    {
        const Entity eventEntity = gCoordinator.createEntity();
        const auto position = gCoordinator.getComponent<TransformComponent>(enemyEntity).position;
        ObstacleData obstacleData{position.x, position.y};
        const auto synchronisedEvent = SynchronisedEvent{.updateType = SynchronisedEvent::UpdateType::STATE,
                                                         .variant = SynchronisedEvent::Variant::SEND_SPAWNERS_POSITIONS,
                                                         .obstacleData = obstacleData,
                                                         .updatedEntity = enemyEntity};
        gCoordinator.addComponent(eventEntity, synchronisedEvent);
    }
}

void SpawnerSystem::spawnOnDemand(const comm::EnemyPositionsUpdate& enemiesToSpawn) const
{
    for (auto enemyToSpawn : enemiesToSpawn.enemy_positions())
    {
        const Entity newEnemyEntity = gCoordinator.createEntity();

        if (gCoordinator.mapEntity(enemyToSpawn.id(), newEnemyEntity))
        {
            spawnEnemy(newEnemyEntity, enemyToSpawn);
            SpawnerComponent spawnerComponent{};
            spawnerComponent.enemyType = Enemies::EnemyType::MELEE;
        }
        else
        {
            std::cout << "Couldn't spawn enemies because id is occupied!!!\n";
        }
    }
}
