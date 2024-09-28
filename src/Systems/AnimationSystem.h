#pragma once
#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    AnimationSystem();
    void init();
    void update(float deltaTime);
    void updateEntityAnimation(Entity, float deltaTime);
    int calculateFrameDuration(const AnimationComponent&);
    [[nodiscard]] bool isTimeForNextFrame(Entity entity);
    void loadNextFrame(Entity, AnimationComponent&);

private:
    float m_frameTime{};
};