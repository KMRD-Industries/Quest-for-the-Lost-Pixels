#include "EnemySystem.h"
#include <random>
#include "ColliderComponent.h"
#include "Config.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "MultiplayerSystem.h"
#include "Physics.h"
#include "PublicConfigMenager.h"
#include "SpawnerSystem.h"
#include "TransformComponent.h"

extern PublicConfigSingleton configSingleton;

EnemySystem::EnemySystem()
{
    gen.seed(gCoordinator.getRegisterSystem<MultiplayerSystem>()->getSeed()); // Seed the generator
    dis = std::uniform_real_distribution<float>(-1, 1); // Range between -1 and 1
}

void EnemySystem::deleteEnemies() const
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            colliderComponent->toDestroy = true;
        else
            entitiesToKill.push_back(entity);

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}
