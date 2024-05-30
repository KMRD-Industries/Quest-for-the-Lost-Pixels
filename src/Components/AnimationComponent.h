#pragma once

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "AnimationFrame.h"
#include "AtlasComponents/Texture.h"

struct AnimationComponent
{
    AnimationComponent() : it(frames.end()) {}
    std::vector<AnimationFrame> frames;
    std::vector<AnimationFrame>::iterator it;
    bool loop_animation = true;

    explicit AnimationComponent(const std::vector<AnimationFrame>& frames) :
        frames(frames), it(this->frames.end()), loop_animation{true} {};
};
