#pragma once

#include <box2d/box2d.h>
#include "Core/Types.h"
#include "System.h"

struct TransformComponent;

class MyContactListener : public b2ContactListener
{
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

class CollisionSystem : public System
{
public:
    CollisionSystem();
    void update(const float&);
    void createMapCollision();
    void updateSimulation(float timeStep, int32 velocityIterations, int32 positionIterations) const;
    void deleteBody(Entity entity) const;
    void deleteMarkedBodies() const;

private:
    void performFixedUpdate() const;
    MyContactListener m_myContactListenerInstance;
    float m_frameTime{};
};