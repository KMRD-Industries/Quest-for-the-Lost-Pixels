#pragma once
#include "System.h"

class CharacterSystem : public System
{
public:
    CharacterSystem() = default;
    void update();

private:
    void cleanUpDeadEntities();
};