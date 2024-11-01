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

private:
    void updateEntityAnimation(AnimationComponent &, TileComponent &, float) const;
    void advanceFrame(AnimationComponent &) const;
    void updateTileComponent(TileComponent &, const AnimationComponent &) const;

    float m_animationFrameTime{};
    float m_frameTime{};
};
