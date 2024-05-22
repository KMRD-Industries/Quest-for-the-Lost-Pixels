#include "AnimationSystem.h"
#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "TileComponent.h"


extern Coordinator gCoordinator;

void AnimationSystem::updateFrames()
{
    frame_time++;

    for (const auto& entity : m_entities)
    {
        auto& animation_component = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& tile_component = gCoordinator.getComponent<TileComponent>(entity);

        if (!animation_component.frames.empty())
        {
            if (animation_component.it == animation_component.frames.end())
            {
                animation_component.it = animation_component.frames.begin();
            }

            if (frame_time % animation_component.ignore_frames == 0)
            {
                ++animation_component.it;
                if (animation_component.it == animation_component.frames.end())
                {
                    animation_component.it = animation_component.frames.begin();
                }

                tile_component.id = *animation_component.it;
            }
        }
    }
}