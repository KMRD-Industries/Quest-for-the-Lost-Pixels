#pragma once

#include <utility>
#include <vector>
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"
#include "CircularIterator.h"
#include "Helpers.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> frames;
    CircularIterator<std::vector<AnimationFrame>::iterator> it;
    [[maybe_unused]] bool loop_animation = true;

    AnimationComponent() : it(frames.begin(), frames.end()) {}
    explicit AnimationComponent(const std::vector<AnimationFrame>& frames)
        : frames(frames), it(this->frames.begin(), this->frames.end()) {}
};

