#pragma once
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"

extern Coordinator gCoordinator;

class ObjectCreatorSystem : public System
{
public:
    void update();
private:
    void clear();
    void createBaciscObject(CreateBodyWithCollisionEvent);
    void createProjectile(CreateBodyWithCollisionEvent);
};
