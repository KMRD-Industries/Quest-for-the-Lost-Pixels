#pragma once

#include <vector>
#include "AnimationFrame.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> frames{};
    size_t currentFrame{};
    float timeUntilNextFrame = 0;
    [[maybe_unused]] bool loop_animation = true;
};