#pragma once
#include "System.h"

class CharacterSystem : public System
{
public:
    explicit CharacterSystem() = default;

    void update();
    void init();

private:
    void cleanUpDeadEntities() const;
};
