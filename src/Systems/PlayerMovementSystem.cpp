#include "PlayerMovementSystem.h"
#include "Coordinator.h"
#include "TransformComponent.h"

extern Coordinator gCoordinator;


void PlayerMovementSystem::onMove(const glm::vec2& dir) const
{
    for (const auto& entity : m_entities)
    {
        auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);
        const auto normalizedDir{normalize(dir)};
        transformComponent.position += {normalizedDir.x, normalizedDir.y};
    }
}
