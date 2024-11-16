#include "PlayerMovementSystem.h"
#include <EquipmentComponent.h>
#include "CharacterComponent.h"
#include "Coordinator.h"
#include "FightActionEvent.h"
#include "InputHandler.h"
#include "ItemSystem.h"
#include "RenderComponent.h"
#include "SynchronisedEvent.h"
#include "TextTagComponent.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

extern Coordinator gCoordinator;
extern PublicConfigSingleton configSingleton;

PlayerMovementSystem::PlayerMovementSystem() { init(); }

void PlayerMovementSystem::init() { inputHandler = InputHandler::getInstance(); }

void PlayerMovementSystem::update(const float deltaTime)
{
    if (gCoordinator.getComponent<CharacterComponent>(config::playerEntity).hp <= 0.0) return;

    handleAttack();
    handlePickUpAction();
    handleSpecialKeys();

    if (m_frameTime += deltaTime; m_frameTime >= configSingleton.GetConfig().oneFrameTime * 1000)
    {
        handleMovement();
        m_frameTime -= configSingleton.GetConfig().oneFrameTime * 1000;
    }
}

void PlayerMovementSystem::handleSpecialKeys()
{
    // TODO: special keys
}

void PlayerMovementSystem::handlePickUpAction()
{
    const auto inputHandler{InputHandler::getInstance()};

    if (!inputHandler->isPressed(InputType::PickUpItem)) return;

    gCoordinator.getRegisterSystem<ItemSystem>()->input(config::playerEntity);
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

    // Handle Movement
    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(config::playerEntity);
    const auto& equipment = gCoordinator.getComponent<EquipmentComponent>(config::playerEntity);
    const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
    const auto playerSpeed = glm::vec2{normalizedDir.x * config::playerAcc, normalizedDir.y * config::playerAcc};

    transformComponent.velocity = {playerSpeed.x, playerSpeed.y};

    if (!gCoordinator.hasComponent<EquipmentComponent>(config::playerEntity)) return;

    bool weaponMoved = false;
    if (equipment.slots.contains(GameType::slotType::WEAPON))
    {
        const Entity weaponEntity = equipment.slots.at(GameType::slotType::WEAPON);

        if (auto* weaponComponent = gCoordinator.tryGetComponent<WeaponComponent>(weaponEntity))
        {
            auto& weaponTransformComponent = gCoordinator.getComponent<TransformComponent>(weaponEntity);
            auto& weaponRenderComponent = gCoordinator.getComponent<RenderComponent>(weaponEntity);

            auto& currentMousePos = inputHandler->getMousePosition();

            if (currentMousePos != weaponComponent->pivotPoint)
            {
                weaponMoved = true;
                weaponComponent->pivotPoint = currentMousePos;
                weaponRenderComponent.dirty = true;

                transformComponent.scale = {weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f,
                                            transformComponent.scale.y};
                weaponTransformComponent.scale = {weaponComponent->targetPoint.x <= 0 ? -1.f : 1.f,
                                                  weaponTransformComponent.scale.y};
            }
        }
    }

    if (weaponMoved || dir != glm::vec2{})
    {
        Entity eventEntity = gCoordinator.createEntity();
        gCoordinator.addComponent(
            eventEntity,
            SynchronisedEvent{.updateType = SynchronisedEvent::MOVEMENT, .variant = SynchronisedEvent::PLAYER_MOVED});
    }
}

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};

    if (!inputHandler->isPressed(InputType::Attack)) return;

    const Entity fightAction = gCoordinator.createEntity();
    gCoordinator.addComponent(fightAction, FightActionEvent{.entity = config::playerEntity});

    const Entity movementEvent = gCoordinator.createEntity();
    gCoordinator.addComponent(
        movementEvent,
        SynchronisedEvent{.updateType = SynchronisedEvent::MOVEMENT, .variant = SynchronisedEvent::PLAYER_ATTACKED});
}
