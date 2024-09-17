#include "AnimationComponent.h"
#include "Coordinator.h"
#include "TileComponent.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateEntityAnimation(AnimationComponent &, TileComponent &) const;
    int calculateFrameDuration(AnimationComponent &) const;
    bool isTimeForNextFrame(const int &) const;

private:
    long m_frameTime{};
};
