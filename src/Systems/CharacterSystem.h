#pragma once
#include "System.h"

class CharacterSystem : public System
{
public:
    CharacterSystem() = default;
    void update() const;

private:
    void cleanUpDeadEntities() const;
};