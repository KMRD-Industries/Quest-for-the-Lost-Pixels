#pragma once
#include "AnimationComponent.h"
#include "Coordinator.h"
#include "TileComponent.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    AnimationSystem() = default;

    void update(const float &deltaTime) const;
    void updateEntityAnimation(AnimationComponent &, TileComponent &, float) const;
    void advanceFrame(AnimationComponent &animationComponent) const;
    void updateTileComponent(TileComponent &tileComponent, const AnimationComponent &animationComponent) const;

private:
    float m_animationFrameTime{};
    float m_frameTime{};
};