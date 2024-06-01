#pragma once

#include "GameTypes.h"
#include "Tileset.h"

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};
    std::string tag{};
    std::function<void(GameType::CollisionData)> collisionReaction;
    Collision collisionDescription;

    ColliderComponent() = default;

    explicit ColliderComponent(b2Body* body, std::string tag,
                               const std::function<void(GameType::CollisionData)>& collisionReaction) :
        body{body}, tag{std::move(tag)}, collisionReaction{collisionReaction}
    {
    }
};
