#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateFrameTime();
    void updateEntityAnimation(const Entity) const;
    static int calculateFrameDuration(const AnimationComponent&);
    bool isTimeForNextFrame(const int frameDuration) const;
    static void loadNextFrame(const Entity, AnimationComponent&);

private:
    long frame_time{};
};