#pragma once

#include "System.h"

class PassageSystem : public System
{
public:
    void update();
    void clearPassages() const;
    void setPassages(bool) const;
};
