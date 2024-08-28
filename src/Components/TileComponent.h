#pragma once

#include <SFML/System/Vector2.hpp>
#include <utility>

struct TileComponent
{
    long id{};
    std::string tileset;
    int layer{};


    explicit TileComponent(const uint32_t tile_id) : id{static_cast<long>(tile_id)} {};

    TileComponent(const uint32_t tile_id, std::string tileSet, const int layer) :
        id{static_cast<long>(tile_id)}, tileset{std::move(tileSet)}, layer{layer}
    {
    }


    TileComponent() = default;
};
