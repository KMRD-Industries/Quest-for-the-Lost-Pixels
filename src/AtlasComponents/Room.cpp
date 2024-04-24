//
// Created by dominiq on 3/28/24.
//

#include "Room.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

// Given path to json map object load room layout
int Room::load(std::string& path) {
    std::ifstream jsonFile(path);

    if (!jsonFile.is_open()) {
        std::cerr << "Failed to open json map file." << std::endl;
        return 0;
    }

     nlohmann::json data = nlohmann::json::parse(jsonFile);

     compressionLevel = data["compressionlevel"];
     height = data["height"];
     width =  data["width"];
     infinite = data["infinite"];
     nextLayerId = data["nextlayerid"];
     nextObjectId = data["nextobjectid"];
     orientation = data["orientation"];
     renderOrder = data["renderorder"];
     tiledVersion = data["tiledversion"];
     tileHeight = data["tileheight"];
     tileWidth = data["tilewidth"];

     for (auto & i : data["layers"]){
         Layer newLayer = *new Layer(i);
         layers.emplace_back(newLayer);
     }

     return 1;
}