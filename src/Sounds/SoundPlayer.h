#pragma once

#include <SFML/Audio.hpp>
#include "GameTypes.h"

struct SoundPlayer
{
    SoundPlayer() = default;

    explicit SoundPlayer(const Sound::Type type, const sf::SoundBuffer& buffer)
        : soundType(type)
    {
        sound.setBuffer(buffer);
    }

    sf::Sound sound;
    Sound::Type soundType;
};