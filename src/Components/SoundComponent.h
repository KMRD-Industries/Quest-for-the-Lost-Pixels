#pragma once

#include <string>
#include <SFML/Audio.hpp>

struct SoundComponent
{
    std::shared_ptr<sf::SoundBuffer> buffer{};
    std::shared_ptr<sf::Sound> sound{};
    bool isLooping{};
    float volume{};
    bool toPlay{};
    float delay{};

    SoundComponent() = default;

    explicit SoundComponent(const std::string& soundFilePath, const bool toPlay = true, const bool loop = false,
                            const float volume = 100.0f,
                            const float delay = 0.f)
        : isLooping{loop}, volume{volume}, toPlay{toPlay}, delay{delay}
    {
        buffer = std::make_shared<sf::SoundBuffer>();
        if (!buffer->loadFromFile(soundFilePath))
            throw std::runtime_error("Unable to load sound file: " + soundFilePath);
        sound = std::make_shared<sf::Sound>();
        sound->setBuffer(*buffer);
        sound->setLoop(isLooping);
        sound->setVolume(volume);
    }
};