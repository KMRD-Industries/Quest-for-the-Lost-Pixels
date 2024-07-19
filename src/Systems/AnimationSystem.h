#include "AnimationComponent.h"
#include "Coordinator.h"

extern Coordinator gCoordinator;

class AnimationSystem : public System
{
public:
    void updateFrames();
    void updateEntityAnimation(Entity) const;
    static int calculateFrameDuration(AnimationComponent&);
    inline bool isTimeForNextFrame(int frameDuration) const;
    static void loadNextFrame(Entity, AnimationComponent&);

private:
    long m_frameTime{};
};