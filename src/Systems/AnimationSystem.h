#pragma once
#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    AnimationSystem();
    void init();
    void update();
    void updateEntityAnimation(Entity);
    int calculateFrameDuration(const AnimationComponent&);
    [[nodiscard]] bool isTimeForNextFrame(int);
    void loadNextFrame(Entity, AnimationComponent&);

private:
    long m_frameTime{};
};