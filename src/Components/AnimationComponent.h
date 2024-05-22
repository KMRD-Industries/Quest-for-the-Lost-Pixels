#pragma once

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "AtlasComponents/Texture.h"

struct AnimationComponent
{
    AnimationComponent() : it(frames.end()) {} // Initialize iterator to end
    std::vector<long> frames;
    std::vector<long>::iterator it;
    long ignore_frames = 10;
};
