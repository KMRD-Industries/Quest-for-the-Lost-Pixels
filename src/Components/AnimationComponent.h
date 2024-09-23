#pragma once

#include <vector>
#include "AnimationFrame.h"
#include "Helpers.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> frames{};
    size_t currentFrame{};
    int timeUntilNextFrame = 0;
    [[maybe_unused]] bool loop_animation = true;
};
