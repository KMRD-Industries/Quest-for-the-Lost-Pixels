#pragma once

#include <SFML/System/Vector2.hpp>

struct TileComponent
{
    long id{};
    std::string tileset;
    int layer{};

    explicit TileComponent(const uint32_t id) : id{id} {};

    TileComponent() = default;
};
