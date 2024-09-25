#include "AnimationSystem.h"

#include "Config.h"
#include "RenderComponent.h"
#include "TileComponent.h"

AnimationSystem::AnimationSystem() { init(); }

void AnimationSystem::init() { m_frameTime = {}; }

void AnimationSystem::update(const float deltaTime) const
{
    for (const auto entity : m_entities)
    {
        auto& animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        updateEntityAnimationFrame(animationComponent, tileComponent, deltaTime);
    }
}

void AnimationSystem::updateEntityAnimationFrame(AnimationComponent& animation, TileComponent& tile,
                                                 const float deltaTime) const
{
    animation.timeUntilNextFrame -= deltaTime * 1000;
    if (!animation.frames.empty() && animation.timeUntilNextFrame <= 0) loadNextFrame(animation, tile);
}

void AnimationSystem::loadNextFrame(AnimationComponent& animation, TileComponent& tile) const
{
    ++animation.currentFrame %= animation.frames.size();
    animation.timeUntilNextFrame = animation.frames[animation.currentFrame].duration;
    tile.id = animation.frames[animation.currentFrame].tileID;
}