#include "SoundSystem.h"

#include "Coordinator.h"
#include "SoundComponent.h"
#include "SoundManager.h"

extern Coordinator gCoordinator;

void SoundSystem::update(float deltaTime)
{
    if (m_entities.empty())
        return;

    SoundManager& soundManager = SoundManager::getInstance();
    for (const auto entity : m_entities)
    {
        auto& soundComponent = gCoordinator.getComponent<SoundComponent>(entity);
        if (soundComponent.proceeded)
            continue;
        if (soundComponent.stopPlaying)
        {
            if (soundComponent.singleSound)
                soundManager.stopSoundByID(soundComponent.id);
            else
                soundManager.stopSoundsByType(soundComponent.soundToPlay);
        }
        else
        {
            soundComponent.id = soundManager.playSound(soundComponent.soundToPlay, soundComponent.volume,
                                                       soundComponent.isLooping);
        }
        soundComponent.proceeded = true;
    }
}