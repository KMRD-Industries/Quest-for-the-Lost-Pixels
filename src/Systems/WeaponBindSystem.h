#pragma once
#include <stack>

#include "System.h"

class WeaponBindSystem : public System
{
public:
    void update();

private:
    std::stack<Entity> m_entityToRemove{};
};