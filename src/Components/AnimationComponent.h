#pragma once

#include <utility>
#include <vector>
#include "AnimationFrame.h"
#include "CircularIterator.h"
#include "AtlasComponents/Texture.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> frames{};
    int currentFrame{};
    //CircularIterator<std::vector<AnimationFrame>::iterator> it;
    [[maybe_unused]] bool loop_animation = true;
};