#pragma once
#include "GameTypes.h"
#include "Helpers.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_world.h"
#include "box2d/b2_fixture.h"

class RayCastCallback : public b2RayCastCallback
{
public:
    RayCastCallback() = default;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, const float fraction) override
    {
        if (fixture)
        {
            const auto bodyData = reinterpret_cast<GameType::CollisionData*>(fixture->GetBody()->GetUserData().
                pointer);
            if (m_ignoreYourself && bodyData->entityID == m_myEntity)
            {
                return -1.0f;
            }
        }
        m_fixture = fixture;
        m_point = point;
        m_normal = normal;
        m_fraction = fraction;
        return fraction;
    }

    b2Fixture* m_fixture{};
    b2Vec2 m_point{};
    b2Vec2 m_normal{};
    float m_fraction{};
    bool m_ignoreYourself{};
    Entity m_myEntity{};
};

class CircleCastCallback : public b2QueryCallback
{
public:
    CircleCastCallback() = default;

    bool ReportFixture(b2Fixture* fixture) override
    {
        if (fixture)
        {
            const auto bodyData = reinterpret_cast<GameType::CollisionData*>(fixture->GetBody()->GetUserData().pointer);
            if (m_ignoreYourself && bodyData->entityID == m_myEntity)
                return true;

            const auto position = fixture->GetBody()->GetPosition();
            if ((position - m_circleCenter).Length() > m_circleRadius)
                return true;
            m_fixtures.push_back(fixture);
        }
        return true;
    }

    std::vector<b2Fixture*> m_fixtures;
    bool m_ignoreYourself{};
    Entity m_myEntity{};
    b2Vec2 m_circleCenter{};
    b2Vec2 m_forwardVector{};
    float m_circleRadius{};
};

class ConeCastCallback : public b2QueryCallback
{
public:
    ConeCastCallback() = default;

    bool ReportFixture(b2Fixture* fixture) override
    {
        if (fixture)
        {
            const auto bodyData = reinterpret_cast<GameType::CollisionData*>(fixture->GetBody()->GetUserData().pointer);
            if (m_ignoreYourself && bodyData->entityID == m_myEntity)
                return true;

            const auto position = fixture->GetBody()->GetPosition();
            auto vectorToTarget = (position - m_circleCenter);
            if (vectorToTarget.Length() > m_circleRadius)
                return true;

            vectorToTarget.Normalize();
            const auto angle = b2Dot(vectorToTarget, m_forwardVector);
            if (angle <= m_angle && angle >= -m_angle)
                m_fixtures.push_back(fixture);
        }
        return true;
    }

    std::vector<b2Fixture*> m_fixtures;
    bool m_ignoreYourself{};
    Entity m_myEntity{};
    b2Vec2 m_circleCenter{};
    b2Vec2 m_forwardVector{};
    float m_circleRadius{};
    float m_angle{};
};

class Physics
{
public:
    Physics(Physics& other) = delete;
    void operator=(const Physics&) = delete;

    static Physics* getInstance()
    {
        if (m_physics == nullptr)
        {
            m_physics = new Physics();
        }
        return m_physics;
    }

    /**
     * \brief 
     * \param point1 starting point of ray
     * \param point2 ening point of ray
     * \param entity entity to ignore, default nothing
     * \return RaycastData of raycasted object 
     */
    static GameType::RaycastData rayCast(const GameType::MyVec2& point1, const GameType::MyVec2& point2,
                                         const int entity = -10)
    {
        const b2Vec2 newPoint1{convertPixelsToMeters(point1.x), convertPixelsToMeters(point1.y)};
        const b2Vec2 newPoint2{convertPixelsToMeters(point2.x), convertPixelsToMeters(point2.y)};
        RayCastCallback callback;
        if (entity >= 0)
        {
            callback.m_myEntity = entity;
            callback.m_ignoreYourself = true;
        }
        getInstance()->getWorld()->RayCast(&callback, newPoint1, newPoint2);
        if (!callback.m_fixture)
            return {0, "", {0, 0}};
        const auto bodyData =
            reinterpret_cast<GameType::CollisionData*>(callback.m_fixture->GetBody()->GetUserData().pointer);
        return {bodyData->entityID, bodyData->tag, callback.m_point};
    }

    static std::vector<GameType::RaycastData> circleCast(const GameType::MyVec2& center, const float radius,
                                                         const int entity = -10)
    {
        const b2Vec2 newCenter{convertPixelsToMeters(center.x), convertPixelsToMeters(center.y)};
        const float newRadius = convertPixelsToMeters(radius);

        b2CircleShape circle;
        circle.m_radius = newRadius;
        circle.m_p = newCenter;

        b2AABB aabb;
        circle.ComputeAABB(&aabb, b2Transform(newCenter, b2Rot(0.0f)), 0);

        CircleCastCallback callback;
        if (entity >= 0)
        {
            callback.m_myEntity = entity;
            callback.m_ignoreYourself = true;
        }
        callback.m_circleCenter = newCenter;
        callback.m_circleRadius = newRadius;

        getInstance()->getWorld()->QueryAABB(&callback, aabb);

        std::vector<GameType::RaycastData> results;

        for (const auto fixture : callback.m_fixtures)
        {
            const auto bodyData =
                reinterpret_cast<GameType::CollisionData*>(fixture->GetBody()->GetUserData().pointer);
            GameType::RaycastData data{bodyData->entityID, bodyData->tag, fixture->GetBody()->GetPosition()};
            results.push_back(data);
        }

        return results;
    }

    static std::vector<GameType::RaycastData> coneCast(const GameType::MyVec2& center,
                                                       const GameType::MyVec2& forwardVector, const float radius,
                                                       const float angle,
                                                       const int entity = -10)
    {
        const b2Vec2 newCenter{convertPixelsToMeters(center.x), convertPixelsToMeters(center.y)};
        const float newRadius = convertPixelsToMeters(radius);
        b2Vec2 newForward = {convertPixelsToMeters(forwardVector.x), convertPixelsToMeters(forwardVector.y)};
        newForward.Normalize();
        b2CircleShape circle;
        circle.m_radius = newRadius;
        circle.m_p = newCenter;

        b2AABB aabb;
        circle.ComputeAABB(&aabb, b2Transform(newCenter, b2Rot(0.0f)), 0);

        ConeCastCallback callback;
        if (entity >= 0)
        {
            callback.m_myEntity = entity;
            callback.m_ignoreYourself = true;
        }
        callback.m_circleCenter = newCenter;
        callback.m_circleRadius = newRadius;
        callback.m_angle = angle;
        callback.m_forwardVector = newForward;

        getInstance()->getWorld()->QueryAABB(&callback, aabb);

        std::vector<GameType::RaycastData> results;

        for (const auto fixture : callback.m_fixtures)
        {
            const auto bodyData =
                reinterpret_cast<GameType::CollisionData*>(fixture->GetBody()->GetUserData().pointer);
            GameType::RaycastData data{bodyData->entityID, bodyData->tag, fixture->GetBody()->GetPosition()};
            results.push_back(data);
        }

        return results;
    }

    static b2World* getWorld()
    {
        return &getInstance()->m_world;
    }

private:
    Physics() :
        m_world(b2Vec2(0.f, 0.f))
    {
    }

    b2World m_world;
    static Physics* m_physics;
};
