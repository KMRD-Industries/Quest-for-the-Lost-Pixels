#include "PlayerMovementSystem.h"
#include "Coordinator.h"
#include "TransformComponent.h"
#include "glm/glm.hpp"
extern Coordinator gCoordinator;


void PlayerMovementSystem::onMove(const glm::vec2& dir)
{
    for (const auto& entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto normalizedDir{glm::normalize(dir)};
        transformComponent.position += {normalizedDir.x, normalizedDir.y};
    }
}
