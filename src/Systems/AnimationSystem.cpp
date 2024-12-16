#include "AnimationSystem.h"

#include <DirtyFlagComponent.h>

#include "Config.h"
#include "TileComponent.h"

void AnimationSystem::update(const float &deltaTime) const
{
    std::vector<Entity> entitiesToRemove;

    for (const auto entity : m_entities)
    {
        auto &animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        if (animationComponent.frames.empty()) continue;

        if (updateEntityAnimation(entity, deltaTime))
        {
            entitiesToRemove.push_back(entity);
        }
    }

    for (auto &entity : entitiesToRemove)
    {
        gCoordinator.destroyEntity(entity);
    }
}

bool AnimationSystem::updateEntityAnimation(const Entity entity, const float deltaTime) const
{
    auto &animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
    auto &tileComponent = gCoordinator.getComponent<TileComponent>(entity);

    animationComponent.timeUntilNextFrame -= deltaTime;

    if (animationComponent.timeUntilNextFrame <= 0)
    {
        auto frames = animationComponent.frames.at(animationComponent.currentState);
        if (frames.size() <= 0) return false;

        if (animationComponent.loop_animation == false && animationComponent.currentFrame == frames.size() - 1)
        {
            return true;
        }
        else
        {
            // Update animation by replacing current tile with next in animation order
            animationComponent.currentFrame = (animationComponent.currentFrame + 1) % frames.size();
            animationComponent.timeUntilNextFrame = frames[animationComponent.currentFrame].duration;
            tileComponent.id = frames[animationComponent.currentFrame].tileID;

            if (animationComponent.stateToAnimationLookup.contains(animationComponent.currentState))
                tileComponent.tileSet =
                    animationComponent.stateToAnimationLookup.at(animationComponent.currentState).first;

            // Notify Renderer that tile has been changed!
            gCoordinator.addComponentIfNotExists(entity, DirtyFlagComponent{});
        }
    }

    return false;
}
