#pragma once

#include <functional>
#include <string>
#include <utility>
#include "GameTypes.h"
#include "Tileset.h"
#include "box2d/b2_fixture.h"

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};
    b2Fixture* fixture{nullptr};

    std::string tag{};
    std::function<void(GameType::CollisionData)> onCollisionEnter;
    std::function<void(GameType::CollisionData)> onCollisionOut;
    Collision collision{};
    Collision specialCollision{};
    bool toDestroy{false};

    ColliderComponent() = default;

    explicit ColliderComponent(Collision collision) : collision(std::move(collision)) {}
    explicit ColliderComponent(b2Body* body, std::string tag,
                               const std::function<void(GameType::CollisionData)>& onCollisionEnter,
                               const std::function<void(GameType::CollisionData)>& onCollisionOut) :
        body{body}, tag{std::move(tag)}, onCollisionEnter{onCollisionEnter}, onCollisionOut{onCollisionOut}
    {
    }
};
