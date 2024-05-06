#pragma once

#include <string>

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

