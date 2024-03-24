#pragma once
#include "Types.h"

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(const Entity entity) = 0;
};
