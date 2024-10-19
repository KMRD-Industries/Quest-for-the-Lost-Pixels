#include "SoundSystem.h"

#include "Coordinator.h"
#include "SoundComponent.h"
#include "SoundManager.h"

extern Coordinator gCoordinator;

void SoundSystem::update(float deltaTime)
{
    if (m_entities.empty())
        return;

    std::deque<Entity> entitiesToRemove;

    SoundManager& soundManager = SoundManager::getInstance();
    for (const auto entity : m_entities)
    {
        auto& soundComponent = gCoordinator.getComponent<SoundComponent>(entity);
        if (soundComponent.stopPlaying)
            soundManager.stopSoundsByType(soundComponent.soundToPlay);
        else
            soundManager.playSound(soundComponent.soundToPlay, soundComponent.volume, soundComponent.isLooping);
        entitiesToRemove.push_back(entity);
    }
    while (!entitiesToRemove.empty())
    {
        gCoordinator.destroyEntity(entitiesToRemove.front());
        entitiesToRemove.pop_front();
    }
}