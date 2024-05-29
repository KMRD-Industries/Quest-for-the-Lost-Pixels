#pragma once

#include <box2d/box2d.h>
#include <functional>

#include "Coordinator.h"
#include "Core/Types.h"
#include "GameTypes.h"
#include "System.h"
#include "glm/vec2.hpp"


class MyContactListener : public b2ContactListener
{
    void BeginContact(b2Contact* contact) override;

    void EndContact(b2Contact* contact) override;
};

class CollisionSystem : public System
{
public:
    explicit CollisionSystem() :
        m_world(b2Vec2(0.f, 0.f)) { m_world.SetContactListener(&m_myContactListenerInstance); }

    void updateCollision() const;
    void updateSimulation(float timeStep, int32 velocityIterations, int32 positionIterations);
    void createBody(
        Entity entity, const std::string& tag, const glm::vec2& colliderSize = {},
        const std::function<void(GameType::CollisionData)>& onCollisionEnter = [](const GameType::CollisionData&)
        {
        },
        const std::function<void(GameType::CollisionData)>& onCollisionOut = [](const GameType::CollisionData&)
        {
        },
        bool isStatic = true, bool useTextureSize = true);
    void deleteBody(Entity entity);

private:
    MyContactListener m_myContactListenerInstance;
    b2World m_world;
};
