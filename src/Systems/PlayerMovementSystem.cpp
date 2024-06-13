#include "PlayerMovementSystem.h"
#include "CharacterComponent.h"
#include "Coordinator.h"
#include "InputHandler.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "glm/detail/func_geometric.inl"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update() const
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

    for (const auto& entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        constexpr int playerAcc = 300;
        const auto playerSpeed = glm::vec2{normalizedDir.x * playerAcc, normalizedDir.y * playerAcc};

        transformComponent.scale = {(flip) ? -1.f : 1.f, transformComponent.scale.y};
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};
    for (const auto& entity : m_entities)
    {
        if (!inputHandler->isPressed(InputType::Attack))
            continue;

        auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto bounds = renderComponent.sprite.getGlobalBounds();

        const auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto center = glm::vec2{
            transformComponent.position.x + bounds.width / 2,
            transformComponent.position.y + bounds.height / 2};
        const auto range = glm::vec2{center.x * config::playerAttackRange * transformComponent.scale.x, center.y};

        const auto targetInBox = Physics::rayCast(center, range, entity);
        if (targetInBox.tag == "SecondPlayer")
            gCoordinator.getComponent<CharacterComponent>(targetInBox.entityID).hp -= config::playerAttackDamage;
    }
}
