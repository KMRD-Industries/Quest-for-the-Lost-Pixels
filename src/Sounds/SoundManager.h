#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SFML/Audio.hpp>
#include "GameTypes.h"
#include "SoundPlayer.h"

using SoundID = uint32_t;

class SoundManager
{
public:
    static SoundManager& getInstance()
    {
        static SoundManager instance;
        return instance;
    }

    bool loadSound(const Sound::Type type, const std::string& filename)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename))
        {
            std::cerr << "Failed to load sound: " << filename << std::endl;
            return false;
        }

        m_loadedSounds[type] = buffer;
        return true;
    }

    SoundID playSound(Sound::Type type, const float volume = 100.f, const bool loop = false)
    {
        auto bufferIt = m_loadedSounds.find(type);
        if (bufferIt == m_loadedSounds.end())
        {
            std::cerr << "Sound buffer not found for Type: " << static_cast<int>(type) << std::endl;
            return INVALID_SOUND_ID;
        }

        SoundID newID;

        if (!m_freeIDs.empty())
        {
            newID = *m_freeIDs.begin();
            m_freeIDs.erase(m_freeIDs.begin());
        }
        else
            newID = m_nextID++;

        m_soundComponents[newID] = SoundPlayer(type, bufferIt->second);

        m_soundComponents[newID].sound.setVolume(volume);
        m_soundComponents[newID].sound.setLoop(loop);
        m_soundComponents[newID].sound.play();

        return newID;
    }

    void stopSoundByID(SoundID id)
    {
        auto it = m_soundComponents.find(id);
        if (it != m_soundComponents.end())
        {
            it->second.sound.stop();
            m_soundComponents.erase(it);
            m_freeIDs.insert(id);
        }
        else
            std::cerr << "Sound not found for ID: " << id << std::endl;
    }

    void stopSoundsByType(const Sound::Type type)
    {
        for (auto it = m_soundComponents.begin(); it != m_soundComponents.end();)
            if (it->second.soundType == type)
            {
                it->second.sound.stop();
                m_freeIDs.insert(it->first);
                it = m_soundComponents.erase(it);
            }
            else
                ++it;
    }

    void cleanup()
    {
        for (auto it = m_soundComponents.begin(); it != m_soundComponents.end();)
            if (it->second.sound.getStatus() == sf::Sound::Stopped)
            {
                it = m_soundComponents.erase(it);
                m_freeIDs.insert(it->first);
            }
            else
                ++it;
    }

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

private:
    SoundManager() = default;

    std::unordered_map<Sound::Type, sf::SoundBuffer> m_loadedSounds;
    std::unordered_map<SoundID, SoundPlayer> m_soundComponents;
    std::unordered_set<SoundID> m_freeIDs;
    SoundID m_nextID = 0;

    const SoundID INVALID_SOUND_ID = -1;
};