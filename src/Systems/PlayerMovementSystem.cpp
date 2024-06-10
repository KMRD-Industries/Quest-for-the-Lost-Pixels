#include "PlayerMovementSystem.h"

#include "CharacterComponent.h"
#include "Coordinator.h"
#include "InputHandler.h"
#include "Physics.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update() const
{
    handleMovement();
    handleAttack();
}

void PlayerMovementSystem::handleMovement() const
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
    for (const auto& entity : m_entities)
    {
        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        constexpr int playerAcc = 150;
        const auto playerSpeed = glm::vec2{normalizedDir.x * playerAcc, normalizedDir.y * playerAcc};
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}

void PlayerMovementSystem::handleAttack() const
{
    const auto inputHandler{InputHandler::getInstance()};
    for (const auto& entity : m_entities)
    {
        //if (!inputHandler->isPressed(InputType::Attack))
        //    continue;

        auto renderComponent = gCoordinator.getComponent<RenderComponent>(entity);
        const auto bounds = renderComponent.sprite.getGlobalBounds();

        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto center = glm::vec2{transformComponent.position.x,
                                      transformComponent.position.y};

        const auto forwardPlayerVector = GameType::MyVec2{
            (center.x + 10) * transformComponent.scale.x,
            center.y};
        const auto targetInCircle = Physics::circleCast(center, config::playerAttackRange, entity);
        const auto targetInCone = Physics::coneCast(center, forwardPlayerVector,
                                                    config::playerAttackRange, config::playerAttackAngle, entity);
        if (targetInCircle.size() > 0)
            std::cout << "player\n";
        for (const auto& target : targetInCone)
        {
            if (!gCoordinator.hasComponent<CharacterComponent>(target.entityID))
                continue;
            //gCoordinator.getComponent<CharacterComponent>(target.entityID).hp -= config::playerAttackDamage;
        }
    }
}
