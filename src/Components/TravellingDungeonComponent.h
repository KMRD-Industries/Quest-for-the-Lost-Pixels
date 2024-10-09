#pragma once
#include <deque>
#include <functional>

struct TravellingDungeonComponent {
    int doorsPassed{};
    std::deque<glm::ivec2> moveInDungeon{};
    std::function<void(const glm::ivec2 &)> moveCallback;
};
