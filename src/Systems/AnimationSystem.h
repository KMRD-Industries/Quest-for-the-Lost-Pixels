#pragma once
#include "AnimationComponent.h"
#include "Coordinator.h"
#include "RenderComponent.h"
#include "TileComponent.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    AnimationSystem();
    void init();
    void update(float deltaTime) const;
    void updateEntityAnimationFrame(AnimationComponent&, TileComponent&, float) const;
    void loadNextFrame(AnimationComponent&, TileComponent&) const;

private:
    float m_frameTime{};
};