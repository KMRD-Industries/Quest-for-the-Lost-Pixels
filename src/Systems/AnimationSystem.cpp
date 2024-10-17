#include "AnimationSystem.h"

#include "Config.h"
#include "TileComponent.h"

void AnimationSystem::update(const float &deltaTime) const {
    for (const auto entity: m_entities) {
        auto &animationComponent = gCoordinator.getComponent<AnimationComponent>(entity);
        if (animationComponent.frames.empty()) continue;

        auto &tileComponent = gCoordinator.getComponent<TileComponent>(entity);
        updateEntityAnimation(animationComponent, tileComponent, deltaTime);
    }
}

void AnimationSystem::updateEntityAnimation(AnimationComponent &animationComponent,
                                            TileComponent &tileComponent,
                                            const float deltaTime) const {
    animationComponent.timeUntilNextFrame -= deltaTime;

    while (animationComponent.timeUntilNextFrame <= 0) {
        advanceFrame(animationComponent);
        updateTileComponent(tileComponent, animationComponent);
    }
}

void AnimationSystem::advanceFrame(AnimationComponent &animationComponent) const {
    animationComponent.currentFrame = (animationComponent.currentFrame + 1) % animationComponent.frames.size();
    animationComponent.timeUntilNextFrame = animationComponent.frames[animationComponent.currentFrame].duration;
}

void AnimationSystem::updateTileComponent(TileComponent &tileComponent,
                                          const AnimationComponent &animationComponent) const {
    tileComponent.id = animationComponent.frames[animationComponent.currentFrame].tileID;
}