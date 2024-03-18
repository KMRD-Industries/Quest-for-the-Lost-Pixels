#pragma once

#include "Types.h"

#include <unordered_set>

class System
{
public:
	std::unordered_set<Entity> m_entities;
};