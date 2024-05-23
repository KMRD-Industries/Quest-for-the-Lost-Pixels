#pragma once

#include <SFML/System/Vector2.hpp>
#include <utility>

struct TileComponent
{
    long id{};
    std::string tileset;
    int layer{};
    bool player = false;

    TileComponent(uint32_t id) : id{static_cast<long>(id)} {}
    TileComponent(uint32_t id, std::string tileset, int layer, bool player) :
        id{static_cast<long>(id)}, tileset{std::move(tileset)}, layer{layer}, player{player}
    {
    }
    TileComponent() = default;
};
