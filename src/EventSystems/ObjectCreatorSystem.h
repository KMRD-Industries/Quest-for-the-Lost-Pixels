#pragma once
#include "Coordinator.h"

extern Coordinator gCoordinator;

class ObjectCreatorSystem : public System
{
public:
    void update();
    void clear();
};
