#include "AnimationSystem.h"
#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void AnimationSystem::updateFrames()
{
    m_frameTime = (m_frameTime + 1) % config::frameCycle;

    for (const auto entity : m_entities)
    {
        updateEntityAnimation(entity);
    }
}

void AnimationSystem::updateEntityAnimation(const Entity entity)
{
    if (auto& animation = gCoordinator.getComponent<AnimationComponent>(entity);
        !animation.frames.empty() && isTimeForNextFrame(calculateFrameDuration(animation)))
    {
        loadNextFrame(entity, animation);
    }
}

inline int AnimationSystem::calculateFrameDuration(AnimationComponent& animation)
{
    // Ensure that the division and the addition are done in long, then cast to int
    const long duration = animation.it->duration;

    // Explicitly cast to int after all calculations to avoid narrowing conversions
    return static_cast<int>(duration / static_cast<long>(config::oneFrameTime) + 1);
}

inline bool AnimationSystem::isTimeForNextFrame(const int frameDuration) const
{
    return m_frameTime % frameDuration == 0;
}

void AnimationSystem::loadNextFrame(const Entity entity, AnimationComponent& animation)
{
    auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
    auto& transformComponent = gCoordinator.getComponent<TransformComponent>(entity);

    ++animation.it;

    tileComponent.id = animation.it->tileID;
    transformComponent.rotation = animation.it->rotation;
}
