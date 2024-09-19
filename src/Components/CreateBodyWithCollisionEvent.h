#pragma once
#include <utility>

#include "GameTypes.h"
#include "Types.h"
#include "glm/vec2.hpp"

struct CreateBodyWithCollisionEvent
{
    Entity entity {};
    std::string tag;
    glm::vec2 colliderSize {};
    std::function<void(GameType::CollisionData)> onCollisionEnter {};
    std::function<void(GameType::CollisionData)> onCollisionOut {};
    bool isStatic {};
    bool useTextureSize {};
    glm::vec2 offset {};

    CreateBodyWithCollisionEvent()
      : entity(0),
        tag(""),
        colliderSize(0.0f, 0.0f),
        onCollisionEnter([](const GameType::CollisionData&) {}),
        onCollisionOut([](const GameType::CollisionData&) {}),
        isStatic(true),
        useTextureSize(true),
        offset(0.0f, 0.0f)
    {}

    CreateBodyWithCollisionEvent(
        const Entity entity, std::string  tag, const glm::vec2& colliderSize = {},
        const std::function<void(GameType::CollisionData)>& onCollisionEnter = [](const GameType::CollisionData&) {},
        const std::function<void(GameType::CollisionData)>& onCollisionOut = [](const GameType::CollisionData&) {},
        const bool isStatic = true, const bool useTextureSize = true, const glm::vec2& offset = {0., 0.}):
    entity{entity},
    tag{std::move(tag)},
    colliderSize{colliderSize},
    onCollisionEnter{onCollisionEnter},
    onCollisionOut{onCollisionOut},
    isStatic{isStatic},
    useTextureSize{useTextureSize},
    offset{offset}
    {
    }
};