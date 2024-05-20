#include "PlayerMovementSystem.h"
#include <iostream>
#include "Coordinator.h"
#include "InputHandler.h"
#include "TransformComponent.h"
#include "glm/detail/func_geometric.inl"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update() const { handleMovement(); }

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
        const auto noInput{dir.x == 0 && dir.y == 0};
        const auto normalizedDir{normalize(dir)};
        const auto playerSpeed = glm::vec2{normalizedDir.x * 5, normalizedDir.y * 5};
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
        if (noInput)
        {
            return;
        }
        transformComponent.position += {playerSpeed.x, playerSpeed.y};
    }
}
