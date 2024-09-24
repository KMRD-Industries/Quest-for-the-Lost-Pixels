#include "AnimationSystem.h"

#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

AnimationSystem::AnimationSystem() { init(); }

void AnimationSystem::init() { m_frameTime = {}; }

void AnimationSystem::update()
{
    m_frameTime = (m_frameTime + 1) % config::frameCycle;

    for (const auto entity : m_entities) updateEntityAnimation(entity);
}

void AnimationSystem::updateEntityAnimation(const Entity entity)
{
    if (auto& animation = gCoordinator.getComponent<AnimationComponent>(entity);
        !animation.frames.empty() && isTimeForNextFrame(calculateFrameDuration(animation)))
        loadNextFrame(entity, animation);
}

int AnimationSystem::calculateFrameDuration(const AnimationComponent& animation)
{
    // Ensure that the division and the addition are done in long, then cast to int
    const long duration{animation.frames[animation.currentFrame].duration};
    return static_cast<int>(duration / static_cast<long>(config::oneFrameTime) + 1);
}

bool AnimationSystem::isTimeForNextFrame(const int frameDuration) { return m_frameTime % frameDuration == 0; }

void AnimationSystem::loadNextFrame(const Entity entity, AnimationComponent& animation)
{
    auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
    ++animation.currentFrame %= animation.frames.size();
    tileComponent.id = animation.frames[animation.currentFrame].tileID;
}