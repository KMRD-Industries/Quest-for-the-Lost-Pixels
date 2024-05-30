#pragma once

#include "AnimationFrame.h"
#include <utility>
#include <vector>

struct AnimationComponent
{
    AnimationComponent() : it(frames.end()) {}
    std::vector<AnimationFrame> frames;
    std::vector<AnimationFrame>::iterator it;
    bool loop_animation = true;

    explicit AnimationComponent(const std::vector<AnimationFrame>& frames) :
        frames(frames), it(this->frames.end()) {};
};
