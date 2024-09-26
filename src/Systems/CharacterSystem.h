#pragma once
#include "System.h"

class CharacterSystem : public System
{
public:
    explicit CharacterSystem() = default;
    void update() const;

private:
    void cleanUpDeadEntities() const;
};
