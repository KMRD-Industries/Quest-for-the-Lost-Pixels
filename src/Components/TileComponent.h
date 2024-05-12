#pragma once

#include <SFML/System/Vector2.hpp>

struct TileComponent
{
    long id = 0;
    std::string tileset;
    int layer = 0;

    explicit TileComponent(const uint32_t id) : id(id), tileset() {}

    TileComponent() = default;
};
