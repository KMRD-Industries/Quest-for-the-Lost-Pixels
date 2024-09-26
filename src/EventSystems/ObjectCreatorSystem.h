#pragma once
#include "Coordinator.h"
#include "CreateBodyWithCollisionEvent.h"
#include "box2d/b2_body.h"

extern Coordinator gCoordinator;

class ObjectCreatorSystem : public System
{
public:
    void update();
    [[nodiscard]] b2BodyDef defineBody(const CreateBodyWithCollisionEvent& eventInfo) const;
    [[nodiscard]] b2FixtureDef defineFixture(const CreateBodyWithCollisionEvent& eventInfo) const;
    [[nodiscard]] b2PolygonShape defineShape(const CreateBodyWithCollisionEvent& eventInfo) const;
    void clear();

private:
    void createBasicObject(const CreateBodyWithCollisionEvent& eventInfo) const;
    void createProjectile(const CreateBodyWithCollisionEvent& eventInfo) const;
};
