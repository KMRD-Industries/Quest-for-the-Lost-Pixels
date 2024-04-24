#ifndef LAYER_H
#define LAYER_H

#include <nlohmann/json.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Tile.h"

struct layer {
    int id;
    std::string name;
    std::string className;
    int x;
    int y;
    double width;
    double height;
    double opacity;
    bool visible;
    std::string tintcolor;
    double offsetX;
    double offsetY;
    double parallaXX;
    double parallaXY;
};

class Layer {
   public:
    explicit Layer(const nlohmann::json&);
    void loadLayer(const nlohmann::json&);
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getTiles() const { return tiles; }

   private:
    layer m_layer;
    int id = 0;

    std::vector<std::shared_ptr<Tile>> tiles; // Assuming Tile is a valid type
};

#endif // LAYER_H
