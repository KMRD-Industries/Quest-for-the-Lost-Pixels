#include "PlayerMovementSystem.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "EquipWeaponSystem.h"
#include "EquippedWeaponComponent.h"
#include "InputHandler.h"
#include "InventorySystem.h"
#include "Physics.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "TextTagComponent.h"
#include "TileComponent.h"
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

    if (inputHandler->isHeld(InputType::MoveUp)) // Move Up
    {
        dir.y -= 1;
    }

    if (inputHandler->isHeld(InputType::MoveDown)) // Move Down
    {
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
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}

void PlayerMovementSystem::handleMousePositionUpdate() const
{
    const auto inputHandler{InputHandler::getInstance()};

    for (const auto entity : m_entities)
    {
        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);
        weaponComponent.pivotPoint = inputHandler->getMousePosition();

        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        transformComponent.scale = {weaponComponent.targetPoint.x <= 0 ? -1.f : 1.f, transformComponent.scale.y};
    }
}

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};
    for (const auto& entity : m_entities)
    {
        if (!inputHandler->isPressed(InputType::Attack)) continue;

        // Start attack animation
        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
        const auto& renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);

        weaponComponent.isFacingRight = renderComponent.sprite.getScale().x > 0;
        weaponComponent.queuedAttack = true;

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

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

        const auto targetInBox = Physics::rayCast(center, point2, entity);

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
