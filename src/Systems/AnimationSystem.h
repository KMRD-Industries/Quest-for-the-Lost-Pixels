#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();

private:
    long ignore_frames = 30;
    long frame_time;
};
