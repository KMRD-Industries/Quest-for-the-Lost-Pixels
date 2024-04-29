//
// Created by dominiq on 4/27/24.
//

#include <optional>
#include <string>
#ifndef SF_ROOMCOMPONENT_H
#define SF_ROOMCOMPONENT_H

#endif  // SF_ROOMCOMPONENT_H

enum class Orientation{
    Orthogonal,
    Isometric,
    Staggered,
    Hexagonal,
    None
};

enum class RenderOrder{
    RightDown,
    RightUp,
    LeftDown,
    LeftUp,
    None
};

enum class StaggerAxis{
    X, Y, None
};

enum class StaggerIndex{
    Even, Odd, None
};

struct RoomComponent {
    double width = 0;
    double height = 0;
    int tileHeight = 16;
    int tileWidth = 16;
    std::string orientation = "None";
    std::string renderOrder = "None";
    std::string tiledVersion;
    int compressionLevel = 0;
    int nextLayerId = 0;
    int nextObjectId = 0;
    bool infinite = false;

    RoomComponent() = default;
};