#pragma once
#include "Coordinator.h"

extern Coordinator gCoordinator;

class FightSystem : public System
{
public:
    void update();
};
