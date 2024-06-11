#pragma once

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"

struct AnimationComponent
{
    std::vector<AnimationFrame> frames;
    std::vector<AnimationFrame>::iterator it;
    [[maybe_unused]] bool loop_animation = true;

    AnimationComponent() : it(frames.end()) {}
    explicit AnimationComponent(const std::vector<AnimationFrame>& frames) : frames(frames), it(this->frames.end()){};
};