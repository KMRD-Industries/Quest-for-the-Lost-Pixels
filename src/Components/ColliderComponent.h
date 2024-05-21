#pragma once
#include <string>

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};
    std::string tag{};
    std::function<void(GameType::CollisionData)> collisionReaction;

    ColliderComponent() = default;

    explicit ColliderComponent(b2Body* body, const std::string& tag,
                               const std::function<void(GameType::CollisionData)>& collisionReaction) :
        body{body}, tag{tag}, collisionReaction{collisionReaction}
    {
    }
};
