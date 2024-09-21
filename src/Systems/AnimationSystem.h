#pragma once
#include "AnimationComponent.h"
#include "Coordinator.h"
#include "TileComponent.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void update();
    void init();

private:
    long m_frameTime{};
};
