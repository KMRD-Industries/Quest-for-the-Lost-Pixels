#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateEntityAnimation(Entity);
    int calculateFrameDuration(AnimationComponent&);
    bool isTimeForNextFrame(int frameDuration) const;
    void loadNextFrame(Entity, AnimationComponent&);

private:
    long m_frameTime{};
};
