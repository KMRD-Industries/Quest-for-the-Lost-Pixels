#pragma once

#include <box2d/box2d.h>
#include <functional>

#include "Core/Types.h"
#include "GameTypes.h"
#include "Physics.h"
#include "System.h"
#include "glm/vec2.hpp"

struct TransformComponent;

class MyContactListener : public b2ContactListener
{
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

class CollisionSystem : public System
{
public:
    void init();
    void update(const float&, float, int, int);
    void createMapCollision();
    void performFixedUpdate() const;
    void updateSimulation(float timeStep, int32 velocityIterations, int32 positionIterations) const;

    CollisionSystem();

    void createBody(
        Entity entity, const std::string& tag, const glm::vec2& colliderSize = {},
        const std::function<void(GameType::CollisionData)>& onCollisionEnter = [](const GameType::CollisionData&) {},
        const std::function<void(GameType::CollisionData)>& onCollisionOut = [](const GameType::CollisionData&) {},
        bool isStatic = true, bool useTextureSize = true, const glm::vec2& offset = {0., 0.});

    void deleteBody(Entity entity) const;
    void deleteMarkedBodies() const;

private:
    MyContactListener m_myContactListenerInstance;
    float m_frameTime{};
};