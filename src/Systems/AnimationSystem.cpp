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

void AnimationSystem::updateEntityAnimation(const Entity entity) const
{
    if (auto& animation = gCoordinator.getComponent<AnimationComponent>(entity);
        !animation.frames.empty() && isTimeForNextFrame(calculateFrameDuration(animation)))
    {
        loadNextFrame(entity, animation);
    }
}

inline int AnimationSystem::calculateFrameDuration(AnimationComponent& animation)
{
    const auto duration = static_cast<float>(animation.it->duration);
    return static_cast<int>(duration / config::oneFrameTime) + 1;
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

    tileComponent.id = animation.it->tileid;
    transformComponent.rotation = animation.it->rotation;
}
