#include "AnimationComponent.h"
#include "Coordinator.h"
#include "TileComponent.h"

extern Coordinator gCoordinator;

class ObjectCreatorSystem : public System
{
public:
    void update();
};
