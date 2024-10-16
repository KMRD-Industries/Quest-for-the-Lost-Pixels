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
#include "WeaponsSystem.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

PlayerMovementSystem::PlayerMovementSystem() { init(); }

void PlayerMovementSystem::init() { inputHandler = InputHandler::getInstance(); }

void PlayerMovementSystem::update(const float deltaTime)
{
    handleAttack();
    handlePickUpAction();
    handleSpecialKeys();

    m_frameTime += deltaTime;
    if (m_frameTime <= configSingleton.GetConfig().oneFrameTime)
        return;
    m_frameTime = 0;

    if (m_frameTime += deltaTime; m_frameTime >= config::oneFrameTimeMs)
    {
        handleMovement();
        m_frameTime -= config::oneFrameTimeMs;
    }
}

void PlayerMovementSystem::handleSpecialKeys()
{
    const auto inputHandler{InputHandler::getInstance()};

    if (!inputHandler->isPressed(InputType::DebugMode)) return;
    config::debugMode = !config::debugMode;
}

void PlayerMovementSystem::handlePickUpAction()
{
    const auto inputHandler{InputHandler::getInstance()};

    if (!inputHandler->isPressed(InputType::PickUpItem)) return;

    for (auto const entity : m_entities)
    {
        gCoordinator.getRegisterSystem<ItemSystem>()->input(entity);
    }
}

void PlayerMovementSystem::handleMovement()
{
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

    for (const auto entity : m_entities)
    {
        // Handle Movement
        auto &transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto &equipment = gCoordinator.getComponent<EquipmentComponent>(entity);
        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        const auto playerSpeed = glm::vec2{normalizedDir.x * configSingleton.GetConfig().playerAcc,
                                           normalizedDir.y * configSingleton.GetConfig().playerAcc};

        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};

        if (!gCoordinator.hasComponent<EquipmentComponent>(entity)) continue;

        if (equipment.slots.contains(GameType::slotType::WEAPON))
        {
            const Entity weaponEntity = equipment.slots.at(GameType::slotType::WEAPON);

            if (auto *weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(weaponEntity))
            {
                auto &weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);
                auto &weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);

                weaponComponent->pivotPoint = inputHandler->getMousePosition();
                weaponRenderComponent.dirty = true;

                transformComponent.scale = {weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f,
                                            transformComponent.scale.y};
                weaponTransformComponent.scale = {weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f,
                                                  weaponTransformComponent.scale.y};
            }
        }

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};

    for (const auto& entity : m_entities)
    {
        if (!inputHandler->isPressed(InputType::Attack))
            continue;

        const Entity fightAction = gCoordinator.createEntity();
        gCoordinator.addComponent(fightAction, FightActionEvent{.entity = entity});
    }
}
