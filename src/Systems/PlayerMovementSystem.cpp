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
    const auto noInput{dir.x == 0 && dir.y == 0};
    if (noInput)
    {
        return;
    }
    for (const auto& entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto normalizedDir{normalize(dir)};
        transformComponent.position += {normalizedDir.x, normalizedDir.y};
    }
}
