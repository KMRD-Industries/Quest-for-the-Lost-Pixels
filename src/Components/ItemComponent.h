#pragma once
#include <string>

#include "GameTypes.h"
#include "TileComponent.h"

struct ItemComponent
{
    std::string name{};
    float value{};
    Items::Behaviours behaviour{};
    TileComponent textureData{};
    std::chrono::system_clock::time_point timestamp{};
};