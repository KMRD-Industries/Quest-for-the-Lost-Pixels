#pragma once

#include <vector>
#include "AnimationFrame.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> animationFrames {};
    size_t currentFrame {};
    float timeUntilNextFrame = 0;
    bool loopAnimation {true};
};