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
    bool updateEntityAnimation(Entity entity, float deltaTime) const;

private:
    float m_animationFrameTime{};
    float m_frameTime{};
};