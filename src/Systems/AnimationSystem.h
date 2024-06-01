#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateFrameTime();
    void updateEntityAnimation(Entity) const;
    static int calculateFrameDuration(const AnimationComponent&);
    bool isTimeForNextFrame(int frameDuration) const;
    static void loadNextFrame(Entity, AnimationComponent&);

private:
    long frame_time{};
};
