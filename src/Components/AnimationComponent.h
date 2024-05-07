#pragma once

#include <vector>
#include <iostream>
#include <SFML/System/Vector2.hpp>

struct AnimationComponent {
    std::vector<u_int32_t> frames;
    u_int32_t actual = 0;
    int ignoreframes = 0;

    AnimationComponent(const std::vector<u_int32_t>& frames, u_int32_t actual)
        : frames(frames),
        actual(actual)
    {}

    explicit AnimationComponent(const std::vector<u_int32_t>& frames)
        : frames(frames)
    {
        actual = frames[0];
        ignoreframes = 0;
    }

    AnimationComponent() = default;
};

