//
// Created by dominiq on 4/27/24.
//

#include <string>
#ifndef SF_LAYERCOMPONENT_H
#define SF_LAYERCOMPONENT_H
#endif  // SF_LAYERCOMPONENT_H

struct LayerComponent {
    int id = 0;
    std::string name;
    std::string className;
    int x = 0;
    int y = 0;
    double width = 0;
    double height = 0;
    double opacity = 0;
    bool visible = true;
    std::string tintcolor;
    double offsetX = 0;
    double offsetY = 0;
    double parallaXX = 0;
    double parallaXY = 0;
};

