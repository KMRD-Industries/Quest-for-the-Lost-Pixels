#pragma once

#include <SFML/System/Vector2.hpp>
#include <utility>

struct TileComponent
{
    long id{-1};
    std::string tileset{};
    int layer{};

    TileComponent() = default;

    explicit TileComponent(const uint32_t tile_id) : id{static_cast<long>(tile_id)}
    {
    };

    TileComponent(const uint32_t tile_id, std::string  tileset, const int layer) :
        id{static_cast<long>(tile_id)}, tileset{std::move(tileset)}, layer{layer}
    {
    }

};
