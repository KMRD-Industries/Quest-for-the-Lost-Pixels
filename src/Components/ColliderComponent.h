#pragma once
#include <string>
#include <utility>

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};
    std::string tag{};
    std::function<void(GameType::CollisionData)> collisionReaction;

    ColliderComponent() = default;

    explicit ColliderComponent(b2Body* body, std::string tag,
                               const std::function<void(GameType::CollisionData)>& collisionReaction) :
        body{body}, tag{std::move(tag)}, collisionReaction{collisionReaction}
    {
    }
};
