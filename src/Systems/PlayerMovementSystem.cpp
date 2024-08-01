#include "PlayerMovementSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "EquippedWeaponComponent.h"
#include "InputHandler.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update()
{
    handleMovement();
    handleAttack();
}

void PlayerMovementSystem::handleMovement()
{
    const auto inputHandler{InputHandler::getInstance()};

    glm::vec2 dir{};

    if (inputHandler->isHeld(InputType::MoveUp))
    {
        dir.y -= 1;
    }

    if (inputHandler->isHeld(InputType::MoveDown))
    { // Move Down
        dir.y += 1;
    }

    if (inputHandler->isHeld(InputType::MoveRight)) // Move Right
    {
        flip = false;
        dir.x += 1;
    }

    if (inputHandler->isHeld(InputType::MoveLeft)) // Move Left
    {
        flip = true;
        dir.x -= 1;
    }

    for (const auto entity : m_entities)
    {
        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        const auto playerSpeed = glm::vec2{normalizedDir.x * config::playerAcc, normalizedDir.y * config::playerAcc};
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        transformComponent.scale = {(flip) ? -1.f : 1.f, transformComponent.scale.y};
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}

void PlayerMovementSystem::handleAttack()
{
    const auto inputHandler{InputHandler::getInstance()};
    for (const auto& entity : m_entities)
    {
        if (!inputHandler->isPressed(InputType::Attack)) continue;

        // Start attack animation
        const auto& equippedWeapone = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapone.currentWeapon);
        weaponComponent.isAttacking = true;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto center = glm::vec2{transformComponent.position.x, transformComponent.position.y};
        const auto range = glm::vec2{center.x * config::playerAttackRange * transformComponent.scale.x, center.y};

        const auto targetInBox = Physics::rayCast(center, range, entity);

        if (targetInBox.tag == "SecondPlayer")
        {
            auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(targetInBox.entityID);
            const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(targetInBox.entityID);
            auto& transformComponent = gCoordinator.getComponent<TransformComponent>(targetInBox.entityID);

            characterComponent.attacked = true;
            characterComponent.hp -= config::playerAttackDamage;

            const b2Vec2& attackerPos = gCoordinator.getComponent<ColliderComponent>(entity).body->GetPosition();
            const b2Vec2& targetPos = colliderComponent.body->GetPosition();

            b2Vec2 recoilDirection = targetPos - attackerPos;
            recoilDirection.Normalize(); // Normalize to get a unit vector for consistent recoil magnitude

            const float& recoilMagnitude = 200.0f;
            b2Vec2 recoilVelocity = recoilMagnitude * recoilDirection;

            transformComponent.velocity += {recoilVelocity.x, recoilVelocity.y};

            b2Vec2 newPosition = colliderComponent.body->GetPosition() + 0.25 * recoilDirection;
            colliderComponent.body->SetTransform(newPosition, colliderComponent.body->GetAngle());
        }
    }
}
