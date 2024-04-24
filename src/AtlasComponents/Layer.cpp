//
// Created by dominiq on 3/28/24.
//

#include "Layer.h"

#include <iostream>
#include <vector>
#include "Helpers.cpp"

Layer::Layer(const nlohmann::json &data) {
    loadLayer(data);
}

void Layer::loadLayer(const nlohmann::json &data){
    this->m_layer.id = data["id"];
    this->m_layer.name = data.value("name", "");
    this->m_layer.className = data.value("class", "");
    this->m_layer.x = data.value("x", 0);
    this->m_layer.y = data.value("y", 0);
    this->m_layer.width = data.value("width", 0);
    this->m_layer.height = data.value("height", 0);
    this->m_layer.opacity = data.value("opacity", 1);
    this->m_layer.visible = data.value("visible", 1);
    this->m_layer.offsetX = data.value("offsetX", 0);
    this->m_layer.offsetY = data.value("offsety", 0);
    this->m_layer.parallaXX = data.value("parallaxx", 1);
    this->m_layer.parallaXY = data.value("parallaxy", 1);

    static const std::uint32_t mask = 0xf0000000;
    int index = 0;

    for (uint32_t i : processDataString(
             data["data"],
             static_cast<unsigned long>(this->m_layer.width) *
                 static_cast<unsigned long>(this->m_layer.height),
             0)) {
        uint32_t flipFlags =  (i & mask) >> 28;
        uint32_t tileID = i & ~mask;

        uint32_t tmp = tileID;

        int xPosition = index % static_cast<int>(this->m_layer.width);
        int yPosition = index / static_cast<int>(this->m_layer.width);

        // Adjust tile ID (assuming 0-based indexing)
        tileID--;

        // Extract boolean flags for horizontal and vertical flipping
        bool horizontalFlip = false;
        bool verticalFlip = false;
        bool diagonalFlip = false;

        if(flipFlags != 0){
            horizontalFlip = (tmp >> 3) & 1;
            verticalFlip = (tmp >> 2) & 1;
            diagonalFlip = (tmp >> 1) & 1;
        }

        this->tiles.emplace_back(
            std::make_shared<Tile>(tileID, horizontalFlip, verticalFlip, diagonalFlip, sf::Vector2f(xPosition * 16.0f * 3.0f, yPosition * 16.0f * 3.0f)));

        index++;
    }
}
