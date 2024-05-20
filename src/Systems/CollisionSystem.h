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
    void updateSimulation(const float timeStep, const int32 velocityIterations, const int32 positionIterations);
    void createBody(const Entity entity, const glm::vec2& colliderSize, const bool isStatic = true,
                    const bool useTextureSize = false);

private:
    b2World m_world;
};
