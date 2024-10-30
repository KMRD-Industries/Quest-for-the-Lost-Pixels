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
    std::string tag{};
    std::function<void(GameType::CollisionData)> onCollisionEnter;
    std::function<void(GameType::CollisionData)> onCollisionOut;
    Collision collision{};
    Collision weaponPlacement{};
    Collision helmetPlacement{};
    Collision bodyArmourPlacement{};

    bool toDestroy{false};
    bool trigger{false};

    ColliderComponent() = default;

    explicit ColliderComponent(bool trigger) : trigger(trigger)
    {
    }

    explicit ColliderComponent(bool trigger, std::string tag) : tag(tag), trigger(trigger)
    {
    }

    explicit ColliderComponent(Collision collision) : collision(std::move(collision))
    {
    }
};