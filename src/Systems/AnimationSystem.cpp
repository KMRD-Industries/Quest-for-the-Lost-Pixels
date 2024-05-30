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

        if (animation_component.frames.empty())
        {
            continue;
        }

        auto duration = static_cast<float>(animation_component.it->duration);
        long frameDuration = static_cast<long>(duration / config::oneFrameTime) + 1;

        if (frame_time % frameDuration != 0)
        {
            continue;
        }

        auto& tile_component = gCoordinator.getComponent<TileComponent>(entity);

        ++animation_component.it;

        if (animation_component.it == animation_component.frames.end())
        {
            animation_component.it = animation_component.frames.begin();
        }

        tile_component.id = animation_component.it->tileid;
    }
}