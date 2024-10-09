#pragma once

#include <deque>
#include <functional>
#include <glm/vec2.hpp>

struct PassageComponent
{
    std::deque<glm::ivec2> moveInDungeon{};
    std::function<void()> moveCallback;
    bool activePassage = true;
};
