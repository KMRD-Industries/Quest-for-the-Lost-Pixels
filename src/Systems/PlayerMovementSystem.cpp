#include "PlayerMovementSystem.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "EquippedWeaponComponent.h"
#include "InputHandler.h"
#include "Physics.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update()
{
    handleMousePositionUpdate();
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

        // transformComponent.scale = {(flip) ? -1.f : 1.f, transformComponent.scale.y};
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}

void PlayerMovementSystem::handleMousePositionUpdate() const
{
    const auto inputHandler{InputHandler::getInstance()};

    for (const auto entity : m_entities)
    {
        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);
        weaponComponent.pivot = inputHandler->getMousePosition();
        transformComponent.scale = {(weaponComponent.atan.x <= 0) ? -1.f : 1.f, transformComponent.scale.y};
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
        const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapone.currentWeapon);
        weaponComponent.isAttacking = true;
        weaponComponent.isFacingLeftToRight = renderComponent.sprite.getScale().x > 0;
        weaponComponent.angle = weaponComponent.startingAngle;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto center = glm::vec2{transformComponent.position.x, transformComponent.position.y};
        const auto range = glm::vec2{center.x * config::playerAttackRange * transformComponent.scale.x, center.y};

        const auto targetInBox = Physics::rayCast(center, range, entity);

        if (targetInBox.tag == "SecondPlayer")
        {
            auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(targetInBox.entityID);
            const auto& colliderComponent = gCoordinator.getComponent<ColliderComponent>(targetInBox.entityID);
            auto& secondPlayertransformComponent = gCoordinator.getComponent<TransformComponent>(targetInBox.entityID);

            const Entity tag = gCoordinator.createEntity();
            gCoordinator.addComponent(tag, TextTagComponent{});
            gCoordinator.addComponent(tag, TransformComponent{secondPlayertransformComponent});

            characterComponent.attacked = true;
            characterComponent.hp -= config::playerAttackDamage;

            const b2Vec2& attackerPos = gCoordinator.getComponent<ColliderComponent>(entity).body->GetPosition();
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
}
