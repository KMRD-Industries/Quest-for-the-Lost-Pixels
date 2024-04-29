//
// Created by dominiq on 4/22/24.
//

#ifndef SF_TILECOMPONENT_H
#define SF_TILECOMPONENT_H
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
#endif  // SF_TILECOMPONENT_H
