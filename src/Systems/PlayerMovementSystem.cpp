#include "PlayerMovementSystem.h"

#include "AnimationComponent.h"
#include "CharacterComponent.h"
#include "ColliderComponent.h"
#include "Coordinator.h"
#include "EquipWeaponSystem.h"
#include "EquippedWeaponComponent.h"
#include "FightActionEvent.h"
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

PlayerMovementSystem::PlayerMovementSystem() { init(); }

void PlayerMovementSystem::init() { inputHandler = InputHandler::getInstance(); }

void PlayerMovementSystem::update()
{
    handleMovement();
    handleAttack();
}

void PlayerMovementSystem::handleMovement()
{
    glm::vec2 dir{};

    if (inputHandler->isHeld(InputType::MoveUp)) // Move Up
        dir.y -= 1;

    if (inputHandler->isHeld(InputType::MoveDown)) // Move Down
        dir.y += 1;

    if (inputHandler->isHeld(InputType::MoveRight)) // Move Right
        dir.x += 1;

    if (inputHandler->isHeld(InputType::MoveLeft)) // Move Left
        dir.x -= 1;

    for (const auto entity : m_entities)
    {
        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        const auto playerSpeed = glm::vec2{normalizedDir.x * config::playerAcc, normalizedDir.y * config::playerAcc};

        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};

        if (!gCoordinator.hasComponent<EquippedWeaponComponent>(entity)) continue;

        const auto& equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(entity);
        auto& weaponComponent = gCoordinator.getComponent<WeaponComponent>(equippedWeapon.currentWeapon);
        weaponComponent.pivotPoint = inputHandler->getMousePosition();
        transformComponent.scale = {weaponComponent.targetPoint.x <= 0 ? -1.f : 1.f, transformComponent.scale.y};
    }
}

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};
    for (const auto& entity : m_entities)
    {
        if (!inputHandler->isPressed(InputType::Attack)) continue;

        const Entity fightAction = gCoordinator.createEntity();
        gCoordinator.addComponent(fightAction, FightActionEvent{.entity = entity});
    }
}