#pragma once
#include "System.h"
#include "box2d/b2_math.h"

class CharacterSystem : public System
{
public:
    explicit CharacterSystem()
    {
    }

    void update() const;

private:
    void cleanUpDeadEntities() const;
};
