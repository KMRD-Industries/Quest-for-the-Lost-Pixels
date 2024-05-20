#pragma once
#include "Types.h"

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};
