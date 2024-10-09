#pragma once
#include <utility>

#include "GameTypes.h"
#include "Types.h"
#include "glm/vec2.hpp"

struct CreateBodyWithCollisionEvent
{
    Entity entity{};
    std::string tag;
    glm::vec2 colliderSize{};
    std::function<void(GameType::CollisionData)> onCollisionEnter{};
    std::function<void(GameType::CollisionData)> onCollisionOut{};
    bool isStatic{};
    bool useTextureSize{};
    GameType::ObjectType type{GameType::ObjectType::NORMAL};

    CreateBodyWithCollisionEvent() = default;

    CreateBodyWithCollisionEvent(
        const Entity entity, std::string tag,
        const std::function<void(GameType::CollisionData)> &onCollisionEnter = [](const GameType::CollisionData &) {},
        const std::function<void(GameType::CollisionData)> &onCollisionOut = [](const GameType::CollisionData &) {},
        const bool isStatic = true, const bool useTextureSize = true) :
        entity{entity},
        tag{std::move(tag)},
        onCollisionEnter{onCollisionEnter},
        onCollisionOut{onCollisionOut},
        isStatic{isStatic},
        useTextureSize{useTextureSize}
    {
    }
};
