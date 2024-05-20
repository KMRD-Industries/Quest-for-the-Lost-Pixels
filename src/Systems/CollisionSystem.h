#pragma once

#include <box2d/box2d.h>

#include "Core/Types.h"
#include "System.h"
#include "glm/vec2.hpp"

class CollisionSystem : public System
{
public:
    explicit CollisionSystem() : m_world(b2Vec2(0.f, 0.f)) {}

    void updateCollision() const;
    void updateSimulation(float timeStep, int32 velocityIterations, int32 positionIterations);
    void createBody(Entity entity, const glm::vec2& colliderSize, bool isStatic = true, bool useTextureSize = false);

private:
    b2World m_world;
};
