#include "SoundSystem.h"

#include "Coordinator.h"
#include "SoundComponent.h"

extern Coordinator gCoordinator;

void SoundSystem::update(float deltaTime)
{
    for (const auto entity : m_entities)
    {
        auto soundComponent = gCoordinator.getComponent<SoundComponent>(entity);
        if (soundComponent.toPlay && soundComponent.sound->getStatus() != sf::SoundSource::Playing)
            soundComponent.sound->play();
        else if (!soundComponent.toPlay && soundComponent.sound->getStatus() != sf::SoundSource::Stopped)
            soundComponent.sound->stop();
    }
}