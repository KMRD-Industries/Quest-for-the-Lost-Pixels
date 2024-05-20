#pragma once

class b2Body;

struct ColliderComponent
{
    b2Body* body{nullptr};

    ColliderComponent() = default;

    explicit ColliderComponent(b2Body* body) : body{body} {}
};
