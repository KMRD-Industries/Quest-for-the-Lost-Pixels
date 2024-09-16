#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateEntityAnimation(Entity);
    void loadNextFrame(Entity, AnimationComponent&);
    int calculateFrameDuration(AnimationComponent&);
    bool isTimeForNextFrame(int frameDuration) const;

private:
    long m_frameTime{};
};
