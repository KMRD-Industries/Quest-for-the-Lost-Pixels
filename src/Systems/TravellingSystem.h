#pragma once

#include "System.h"

class TravellingSystem : public System
{
public:
    TravellingSystem() = default;
    void update();
};