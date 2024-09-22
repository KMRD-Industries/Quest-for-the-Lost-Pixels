#pragma once

struct ItemAnimationComponent
{
    float animationDuration{};
    float currentAnimationTime{};
    bool destroy{};
    float startingPositionY{};
    bool shouldAnimate{true};
};