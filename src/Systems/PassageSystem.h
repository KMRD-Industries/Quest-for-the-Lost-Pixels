#pragma once

#include "System.h"

class PassageSystem : public System
{
public:
    void update() const;
    void clearPassages() const;
    void setPassages(const bool) const;
};
