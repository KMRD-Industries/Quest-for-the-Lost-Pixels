#pragma once
#include "GameTypes.h"
#include "Types.h"
#include "glm/vec2.hpp"

struct CreateBodyWithCollisionEvent
{
    Entity entity{};
    std::string tag{};
    glm::vec2 colliderSize{1.0f, 1.0f};
    std::optional<std::function<void(GameType::CollisionData)>> onCollisionEnter = std::nullopt;
    std::optional<std::function<void(GameType::CollisionData)>> onCollisionOut = std::nullopt;
    bool isStatic{true};
    bool useTextureSize{true};
    GameType::ObjectType type{GameType::ObjectType::NORMAL};
};
