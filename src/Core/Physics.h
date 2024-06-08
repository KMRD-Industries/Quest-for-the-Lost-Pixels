#pragma once
#include "GameTypes.h"
#include "Helpers.h"
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
                                         const Entity entity = -10)
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
