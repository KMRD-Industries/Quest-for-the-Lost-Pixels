#pragma once

#include <functional>
#include <string>
#include "GameTypes.h"

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};
    std::string tag{};
    std::function<void(GameType::CollisionData)> onCollisionEnter;
    std::function<void(GameType::CollisionData)> onCollisionOut;
    bool toDestroy{false};

    ColliderComponent() = default;

    explicit ColliderComponent(b2Body* body, const std::string& tag,
                               const std::function<void(GameType::CollisionData)>& onCollisionEnter,
                               const std::function<void(GameType::CollisionData)>& onCollisionOut) :
        body{body}, tag{tag}, onCollisionEnter{onCollisionEnter}, onCollisionOut{onCollisionOut}
    {
    }
};
