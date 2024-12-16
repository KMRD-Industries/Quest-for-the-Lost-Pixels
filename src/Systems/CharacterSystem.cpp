#include "CharacterSystem.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ChestComponent.h"
#include "ColliderComponent.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "EnemyComponent.h"
#include "ItemComponent.h"
#include "Physics.h"
#include "PotionComponent.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;

void CharacterSystem::update() { cleanUpDeadEntities(); }

void CharacterSystem::cleanUpDeadEntities()
{
    std::vector<Entity> entitiesToKill;
    entitiesToKill.reserve(m_entities.size());

    for (const auto entity : m_entities)
    {
        const auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);
        if (characterComponent.hp > 0) continue;

        if (gCoordinator.hasComponent<PotionComponent>(entity))
        {
            if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
            {
                colliderComponent->toDestroy = true;
            }
            else
            {
                entitiesToKill.push_back(entity);
            }

            continue;
        }

        if (gCoordinator.hasComponent<EnemyComponent>(entity) || gCoordinator.hasComponent<ChestComponent>(entity))
        {
            auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);

            if (animationComponent.loop_animation == true) animationComponent.currentFrame = true;
            animationComponent.currentState = AnimationStateMachine::AnimationState::Dead;
            animationComponent.loop_animation = false;

            auto collider = gCoordinator.getComponent<ColliderComponent>(entity).collision;
            gCoordinator.getRegisterSystem<CollisionSystem>()->deleteBody(entity);
            gCoordinator.getComponent<ColliderComponent>(entity).collision = {collider};

            continue;
        }

        if (auto* colliderComponent = gCoordinator.tryGetComponent<ColliderComponent>(entity))
        {
            colliderComponent->toDestroy = true;
        }
        else
        {
            entitiesToKill.push_back(entity);
        }
    }

    for (const auto entity : entitiesToKill) gCoordinator.destroyEntity(entity);
}