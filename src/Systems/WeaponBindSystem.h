#pragma once
#include <stack>

#include "System.h"

class WeaponBindSystem : public System
{
public:
    void update();

private:
    void applyKnockback(const Entity entityID) const;
    std::stack<Entity> m_entityToRemove{};
};