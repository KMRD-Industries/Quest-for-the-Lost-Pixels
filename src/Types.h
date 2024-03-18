#pragma once
#include <bitset>

// ECS
using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

constexpr  Entity MAX_ENTITIES = 5000;
constexpr ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;
