#pragma once

#include "GameTypes.h"

struct SoundComponent
{
    Sound::Type soundToPlay{};
    bool isLooping{false};
    bool singleSound{true};
    float volume{100.f};
    bool stopPlaying{false};
    bool proceeded{false};
    int id{-1};
};