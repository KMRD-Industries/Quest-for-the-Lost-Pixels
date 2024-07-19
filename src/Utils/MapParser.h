#pragma once

#include <fstream>
#include "Map.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

/*
 *  Parse special properties
 * */
void from_json(const json& json, Property& tile_property)
{
    json.at("name").get_to(tile_property.name);
    json.at("type").get_to(tile_property.type);
    json.at("value").get_to(tile_property.value);
}

/*
 * Layer parsing
 * */
void from_json(const json& json, Layer& layer)
{
    json.at("compression").get_to(layer.compression);
    json.at("data").get_to(layer.data);
    json.at("encoding").get_to(layer.encoding);
    json.at("height").get_to(layer.height);
    json.at("id").get_to(layer.id);
    json.at("name").get_to(layer.name);
    json.at("opacity").get_to(layer.opacity);
    json.at("type").get_to(layer.type);
    json.at("visible").get_to(layer.visible);
    json.at("width").get_to(layer.width);
    json.at("x").get_to(layer.x);
    json.at("y").get_to(layer.y);

    // Properties are optional
    if (json.find("properties") != json.end())
    {
        layer.properties = json.at("properties").get<std::vector<Property>>();
    }
}

void from_json(const json& json, Map& map)
{
    json.at("compressionlevel").get_to(map.compressionlevel);
    json.at("height").get_to(map.height);
    json.at("infinite").get_to(map.infinite);
    json.at("layers").get_to(map.layers);
    json.at("nextlayerid").get_to(map.nextlayerid);
    json.at("nextobjectid").get_to(map.nextobjectid);
    json.at("orientation").get_to(map.orientation);
    json.at("renderorder").get_to(map.renderorder);
    json.at("tiledversion").get_to(map.tiledversion);
    json.at("tileheight").get_to(map.tileheight);
    json.at("tilewidth").get_to(map.tilewidth);
    json.at("type").get_to(map.type);
    json.at("version").get_to(map.version);
    json.at("width").get_to(map.width);

    for (const auto& [key, value] : json.at("tilesets").items())
    {
        long first_gid = value.at("firstgid").get<int>();
        const std::string source = value.at("source").get<std::string>();
        map.tilesets.emplace(extractFileName(source, "/", "."), first_gid);
    }
}

Map parseMap(const std::string& path)
{
    try
    {
        nlohmann::json parsed_file;
        std::ifstream jsonFile(path);

        jsonFile >> parsed_file;
        const Map map = parsed_file.get<Map>();

        return map;
    }
    catch (...)
    {
        std::cerr << "Caught an unknown exception" << '\n';
        return {};
    }
}