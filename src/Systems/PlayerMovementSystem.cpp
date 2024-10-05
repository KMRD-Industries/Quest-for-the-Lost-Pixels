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
#include "ItemSystem.h"
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

void PlayerMovementSystem::update(const float deltaTime) {
    handleAttack();
    handlePickUpAction();

    if (m_frameTime += deltaTime; m_frameTime >= config::oneFrameTimeMs) {
        handleMovement();
        m_frameTime -= config::oneFrameTimeMs;
    }
}

void PlayerMovementSystem::handlePickUpAction() {
    const auto inputHandler{InputHandler::getInstance()};

    if (!inputHandler->isPressed(InputType::PickUpItem)) return;

    for (auto const entity: m_entities) {
        gCoordinator.getRegisterSystem<ItemSystem>()->input(entity);
    }
}

void PlayerMovementSystem::handleMovement() {
    const auto inputHandler{InputHandler::getInstance()};

    glm::vec2 dir{};

    if (inputHandler->isHeld(InputType::MoveUp)) // Move Up
        dir.y -= 1;

    if (inputHandler->isHeld(InputType::MoveDown)) // Move Down
        dir.y += 1;

    if (inputHandler->isHeld(InputType::MoveRight)) // Move Right
        dir.x += 1;

    if (inputHandler->isHeld(InputType::MoveLeft)) // Move Left
        dir.x -= 1;

    for (const auto entity: m_entities) {
        // Handle Movement
        auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        const auto &equippedWeapon = gCoordinator.getComponent<EquippedWeaponComponent>(entity);

        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        const auto playerSpeed = glm::vec2{normalizedDir.x * config::playerAcc, normalizedDir.y * config::playerAcc};

        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};

        // Mirroring view if necessary
        if (auto *weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(equippedWeapon.currentWeapon)) {
            auto &weaponTransformComponent =
                    gCoordinator.getComponent<TransformComponent>(equippedWeapon.currentWeapon);
            auto &weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(equippedWeapon.currentWeapon);

            if (!gCoordinator.hasComponent<EquippedWeaponComponent>(entity)) continue;

            weaponComponent->pivotPoint = inputHandler->getMousePosition();
            weaponRenderComponent.dirty = true;

            transformComponent.scale = {weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f, transformComponent.scale.y};
            weaponTransformComponent.scale = {
                weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f,
                weaponTransformComponent.scale.y
            };
        }
    }
}

void PlayerMovementSystem::handleAttack() const {
    const auto inputHandler{InputHandler::getInstance()};

    for (const auto &entity: m_entities) {
        if (!inputHandler->isPressed(InputType::Attack)) continue;

        const Entity fightAction = gCoordinator.createEntity();
        gCoordinator.addComponent(fightAction, FightActionEvent{.entity = entity});
    }
}
