#pragma once

#include <functional>
#include <optional>
#include <string>
#include "GameTypes.h"
#include "Tileset.h"
#include "box2d/b2_fixture.h"

class b2Body;

struct ColliderComponent
{
    b2Body *body{nullptr};
    std::string tag{};
    std::optional<std::function<void(GameType::CollisionData)>> onCollisionEnter{std::nullopt};
    std::optional<std::function<void(GameType::CollisionData)>> onCollisionOut{std::nullopt};
    Collision collision{};
    Collision weaponPlacement{};
    Collision helmetPlacement{};
    Collision bodyArmourPlacement{};
    bool toDestroy{false};
    bool trigger{false};
    bool toRemoveCollider{false};
};