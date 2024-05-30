#include "AnimationSystem.h"
#include "AnimationComponent.h"
#include "Config.h"
#include "RenderComponent.h"
#include "TileComponent.h"

void AnimationSystem::updateFrames()
{
    frame_time = (frame_time + 1) % 60;

    for (const auto& entity : m_entities)
    {
        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);

        // Ignore if there is no animations
        if (animation_component.frames.empty()) continue;

        // Check if animation frame time is up
        if (frame_time % static_cast<long>((animation_component.it->duration * 100 / config::oneFrameTime) + 1))
            continue;

        auto& tile_component = gCoordinator.getComponent<TileComponent>(entity);

        // Get new frame
        ++animation_component.it;

        // Play animation from beginning if frames reach to end
        if (animation_component.it == animation_component.frames.end())
        {
            animation_component.it = animation_component.frames.begin();
        }

        tile_component.id = animation_component.it->tileid;
    }
}