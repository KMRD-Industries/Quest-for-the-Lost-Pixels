#include "AnimationSystem.h"
#include "Config.h"
#include "TileComponent.h"

void AnimationSystem::updateFrames()
{
    updateFrameTime();

    for (const auto entity : m_entities)
    {
        updateEntityAnimation(entity);
    }
}

void AnimationSystem::updateFrameTime() { frame_time = (frame_time + 1) % config::frameCycle; }

void AnimationSystem::updateEntityAnimation(const Entity entity) const
{
    auto& animation = gCoordinator.getComponent<AnimationComponent>(entity);

    if (animation.frames.empty())
    {
        return;
    }

    const int frameDuration = calculateFrameDuration(animation);

    if (!isTimeForNextFrame(frameDuration))
    {
        return;
    }

    loadNextFrame(entity, animation);
}

int AnimationSystem::calculateFrameDuration(const AnimationComponent& animation)
{
    const auto duration = static_cast<float>(animation.it->duration);
    return static_cast<int>(duration / config::oneFrameTime) + 1;
}

bool AnimationSystem::isTimeForNextFrame(const int frameDuration) const { return frame_time % frameDuration == 0; }

void AnimationSystem::loadNextFrame(const Entity entity, AnimationComponent& animation)
{
    auto& tile = gCoordinator.getComponent<TileComponent>(entity);

    if (++animation.it >= animation.frames.end())
    {
        animation.it = animation.frames.begin();
    }

    tile.id = animation.it->tileid;
}