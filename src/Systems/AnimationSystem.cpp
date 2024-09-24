#include "AnimationSystem.h"

#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

AnimationSystem::AnimationSystem() { init(); }

void AnimationSystem::init() { m_frameTime = {}; }

void AnimationSystem::update(const float deltaTime)
{
    for (const auto entity : m_entities) updateEntityAnimation(entity, deltaTime);
}

void AnimationSystem::updateEntityAnimation(const Entity entity, const float deltaTime)
{
    auto& animation = gCoordinator.getComponent<AnimationComponent>(entity);
    animation.timeUntilNextFrame -= deltaTime * 1000;

    if (!animation.frames.empty() && animation.timeUntilNextFrame <= 0) loadNextFrame(entity, animation);
}

void AnimationSystem::loadNextFrame(const Entity entity, AnimationComponent& animation)
{
    auto& tileComponent = gCoordinator.getComponent<TileComponent>(entity);
    ++animation.currentFrame %= animation.frames.size();

    animation.timeUntilNextFrame = animation.frames[animation.currentFrame].duration;
    tileComponent.id = animation.frames[animation.currentFrame].tileID;
}