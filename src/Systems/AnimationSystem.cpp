#include "AnimationSystem.h"
#include "Config.h"
#include "TileComponent.h"
#include "TransformComponent.h"

void AnimationSystem::init() { m_frameTime = {}; }


void AnimationSystem::update()
{
    m_frameTime = (m_frameTime + 1) % config::frameCycle;

    for (const auto entity : m_entities)
    {
        auto& animationComponent        = gCoordinator.getComponent<AnimationComponent>(entity);
        auto& animationTileComponent    = gCoordinator.getComponent<TileComponent>(entity);

        if (!animationComponent.frames.empty())
        {
            if (animationComponent.timeUntilNextFrame--; animationComponent.timeUntilNextFrame <= 0)
            {
                ++animationComponent.it;
                animationTileComponent.id = animationComponent.it->tileID;

                const long duration = animationComponent.it->duration;
                animationComponent.timeUntilNextFrame = static_cast<int>(duration / static_cast<long>(config::oneFrameTime) + 1);
            }
        }
    }
}
