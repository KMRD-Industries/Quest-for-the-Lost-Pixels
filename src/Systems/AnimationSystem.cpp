#include "AnimationSystem.h"
#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void AnimationSystem::updateFrames()
{
    m_frameTime = (m_frameTime + 1) % config::frameCycle;

    for (const auto entity : m_entities)
    {
        auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);

        updateEntityAnimation(animationComponent, tileComponent);
    }
}

void AnimationSystem::updateEntityAnimation(AnimationComponent& animation_component,
                                            TileComponent& tile_component) const
{
    if (!animation_component.frames.empty() && isTimeForNextFrame(calculateFrameDuration(animation_component)))
    {
        ++animation_component.it;
        tile_component.id = animation_component.it->tileID;
    }
}

inline int AnimationSystem::calculateFrameDuration(AnimationComponent& animation) const
{
    const long duration = animation.it->duration;
    return static_cast<int>(duration / static_cast<long>(config::oneFrameTime) + 1);
}

inline bool AnimationSystem::isTimeForNextFrame(const int& frameDuration) const
{
    return m_frameTime % frameDuration == 0;
}
