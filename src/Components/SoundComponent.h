#pragma once

#include "GameTypes.h"

struct SoundComponent
{
    Sound::Type soundToPlay{};
    bool isLooping{false};
    float volume{100.f};
    bool stopPlaying{false};
};