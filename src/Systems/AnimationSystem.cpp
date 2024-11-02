#include "AnimationSystem.h"

#include <DirtyFlagComponent.h>

#include "Config.h"
#include "TileComponent.h"

void AnimationSystem::update(const float &deltaTime) const
{
    for (const auto entity : m_entities)
    {
        auto &animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        if (animationComponent.frames.empty()) continue;

        auto &tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        updateEntityAnimation(entity, deltaTime);
    }
}

void AnimationSystem::updateEntityAnimation(const Entity entity, const float deltaTime) const
{
    auto &animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
    auto &tileComponent = gCoordinator.getComponent<TileComponent>(entity);

    animationComponent.timeUntilNextFrame -= deltaTime;

    if (animationComponent.timeUntilNextFrame <= 0)
    {
        // Update animation by replacing current tile with next in animation order
        animationComponent.currentFrame = (animationComponent.currentFrame + 1) % animationComponent.frames.size();
        animationComponent.timeUntilNextFrame = animationComponent.frames[animationComponent.currentFrame].duration;
        tileComponent.id = animationComponent.frames[animationComponent.currentFrame].tileID;

        // Notify Renderer that tile has been changed!
        gCoordinator.addComponentIfNotExists(entity, DirtyFlagComponent{});
    }
}
