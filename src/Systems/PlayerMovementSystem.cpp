#include "PlayerMovementSystem.h"
#include "Coordinator.h"
#include "InputHandler.h"
#include "TransformComponent.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

void PlayerMovementSystem::update() { handleMovement(); }

void PlayerMovementSystem::handleMovement()
{
    auto* const inputHandler{InputHandler::getInstance()};

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
        const auto normalizedDir = dir == glm::vec2{} ? glm::vec2{} : normalize(dir);
        constexpr int playerAcc = 300;
        const auto playerSpeed = glm::vec2{normalizedDir.x * playerAcc, normalizedDir.y * playerAcc};

        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

        transformComponent.scale = {(flip) ? -1.f : 1.f, transformComponent.scale.y};
        transformComponent.velocity = {playerSpeed.x, playerSpeed.y};
    }
}
