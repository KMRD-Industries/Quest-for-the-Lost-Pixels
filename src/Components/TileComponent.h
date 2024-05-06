#pragma once

#include <SFML/System/Vector2.hpp>

struct TileComponent {
    uint32_t id = 0;
    int layer = 0;

    TileComponent(const uint32_t id, const int layer)
        : id(id),
          layer(layer)
    {}

    TileComponent() = default;

};
