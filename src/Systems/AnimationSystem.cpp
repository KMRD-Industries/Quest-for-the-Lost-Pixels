#include "AnimationSystem.h"

#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void AnimationSystem::init()
{
    m_frameTime = {};
}

void AnimationSystem::update(const float timeStep) const
{
    for (const auto entity : m_entities)
    {
        auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& tileComponent      = gCoordinator.getComponent<TileComponent>(entity);

        if(animationComponent.animationFrames.empty()) continue;
        updateEntityAnimationFrame(animationComponent, tileComponent, timeStep);
    }
}

void AnimationSystem::updateEntityAnimationFrame(AnimationComponent& tileAnimationComponent,
                                                 TileComponent& tileComponent, const float timeStep) const
{
    tileAnimationComponent.timeUntilNextFrame -= timeStep * 1000;
    if (tileAnimationComponent.timeUntilNextFrame <= 0)
        loadNextFrame(tileAnimationComponent, tileComponent);
}

void AnimationSystem::loadNextFrame(AnimationComponent& animation, TileComponent& tile) const
{
    ++animation.currentFrame %= animation.animationFrames.size();
    animation.timeUntilNextFrame = animation.animationFrames[animation.currentFrame].duration;

    tile.id = animation.animationFrames[animation.currentFrame].tileID;
}