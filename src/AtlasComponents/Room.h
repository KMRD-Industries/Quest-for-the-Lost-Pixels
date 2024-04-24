//
// Created by dominiq on 3/28/24.
//

#ifndef ROOM_H
#define ROOM_H
#include <vector>

#include "Layer.h"
#include "Tile.h"


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


class Room {
    private:
        std::optional<double> width;
        std::optional<double> height;
        std::optional<int> tileHeight = 16;
        std::optional<int> tileWidth = 16;
        std::optional<std::string> orientation = "None";
        std::optional<std::string> renderOrder = "None";
        std::optional<std::string> tiledVersion = "";
        std::optional<int> compressionLevel = 0;
        std::optional<int> nextLayerId = 0;
        std::optional<int> nextObjectId = 0;
        std::optional<bool>  infinite = false;
        std::vector<Layer> layers;
    public:
        int load(std::string& path);
        std::vector<Layer> getLayers(){return this->layers;}
};



#endif //ROOM_H
