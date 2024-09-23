#include "FightSystem.h"

#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "CreateBodyWithCollisionEvent.h"
#include "EquippedWeaponComponent.h"
#include "FightActionEvent.h"
#include "ObjectCreatorSystem.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

void FightSystem::update()
{
    for (const auto& entity : m_entities)
    {
        const auto& [eventEntity] = gCoordinator.getComponent<FightActionEvent>(entity);

        // Start attack animation
        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(eventEntity);
        const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(eventEntity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);

        if (weaponComponent.isAttacking) continue;

        weaponComponent.isFacingRight = renderComponent.sprite.getScale().x > 0;
        weaponComponent.queuedAttack = true;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(eventEntity);

        const auto center = glm::vec2{transformComponent.position.x, transformComponent.position.y};

        // Calculate the direction vector using the angle in degrees
        const auto direction = glm::vec2{
            glm::cos(glm::radians(-weaponComponent.targetAngleDegrees)), // Cosine of the angle
            glm::sin(glm::radians(-weaponComponent.targetAngleDegrees)) // Sine of the angle
        };

        // Calculate the range vector based on the direction, attack range, and scale
        const auto range = direction * (config::playerAttackRange * std::abs(transformComponent.scale.x));

        // Calculate point2 using the center and range vector
        const auto point2 = center + range;

        const auto targetInBox = Physics::rayCast(center, point2, eventEntity);

        if (targetInBox.tag == "Enemy")
        {
            auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(targetInBox.entityID);
            const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(targetInBox.entityID);
            auto& secondPlayertransformComponent = gCoordinator.getComponent<TransformComponent>(targetInBox.entityID);

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{});
            gCoordinator.addComponent(tag, TransformComponent{secondPlayertransformComponent});

            characterComponent.attacked = true;
            characterComponent.hp -= config::playerAttackDamage;

            const b2Vec2& attackerPos = gCoordinator.getComponent<ColliderComponent>(eventEntity).body->GetPosition();
            const b2Vec2& targetPos = colliderComponent.body->GetPosition();

            b2Vec2 recoilDirection = targetPos - attackerPos;
            recoilDirection.Normalize();

            const float& recoilMagnitude = 20.0f;
            const b2Vec2 recoilVelocity = recoilMagnitude * recoilDirection;

            secondPlayertransformComponent.velocity.x +=
                static_cast<float>(recoilVelocity.x * config::meterToPixelRatio);
            secondPlayertransformComponent.velocity.y +=
                static_cast<float>(recoilVelocity.y * config::meterToPixelRatio);

            b2Vec2 newPosition = colliderComponent.body->GetPosition() + 0.25 * recoilDirection;
            colliderComponent.body->SetTransform(newPosition, colliderComponent.body->GetAngle());
        }
    }
    {
        std::deque<Entity> entityToRemove;

        for (const auto entity : m_entities)
        {
            entityToRemove.push_back(entity);
        }

        while (!entityToRemove.empty())
        {
            gCoordinator.destroyEntity(entityToRemove.front());
            entityToRemove.pop_front();
        }
    }
}