#pragma once

#include <GameTypes.h>
#include <unordered_map>
#include <vector>
#include "AnimationFrame.h"

struct AnimationComponent
{
    // std::vector<AnimationFrame> frames{};
    size_t currentFrame{};
    float timeUntilNextFrame = 0;
    [[maybe_unused]] bool loop_animation{true};
    AnimationStateMachine::AnimationState currentState = AnimationStateMachine::AnimationState::Idle;
    std::unordered_map<AnimationStateMachine::AnimationState, std::vector<AnimationFrame>> frames;
    std::unordered_map<AnimationStateMachine::AnimationState, std::pair<std::string, long>> stateToAnimationLookup;
};
