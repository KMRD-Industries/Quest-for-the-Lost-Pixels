#pragma once

#include "System.h"
#include "glm/glm.hpp"

class PlayerMovementSystem : public System
{
public:
    void onMove(const glm::vec2& dir);
};
