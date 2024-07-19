#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <functional>

struct PassageComponent
{
    std::deque<glm::ivec2> moveInDungeon{};
    std::function<void()> moveCallback;
};