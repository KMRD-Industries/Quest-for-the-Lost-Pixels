#pragma once

#include <SFML/System/Vector2.hpp>
#include <utility>

struct TileComponent
{
    int64_t id{};
    std::string tileset;
    int layer{};

    explicit TileComponent(const uint32_t tile_id) : id{static_cast<int64_t>(tile_id)} {};

    TileComponent(const uint32_t tile_id, std::string tileset, const int layer) :
        id{static_cast<int64_t>(tile_id)}, tileset{std::move(tileset)}, layer{layer}
    {
    }

    TileComponent() = default;
};
