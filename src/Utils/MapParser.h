#pragma once

#include <fstream>
#include "Map.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

/*
 *  Parse special properties
 * */
void from_json(const json& j, Property& p)
{
    j.at("name").get_to(p.name);
    j.at("type").get_to(p.type);
    j.at("value").get_to(p.value);
}

/*
 * Layer parsing
 */
void from_json(const json& j, Layer& l)
{
    j.at("compression").get_to(l.compression);
    j.at("data").get_to(l.data);
    j.at("encoding").get_to(l.encoding);
    j.at("height").get_to(l.height);
    j.at("id").get_to(l.id);
    j.at("name").get_to(l.name);
    j.at("opacity").get_to(l.opacity);
    j.at("type").get_to(l.type);
    j.at("visible").get_to(l.visible);
    j.at("width").get_to(l.width);
    j.at("x").get_to(l.x);
    j.at("y").get_to(l.y);

    // Properties are optional
    if (j.find("properties") != j.end())
    {
        l.properties = j.at("properties").get<std::vector<Property>>();
    }
}

void from_json(const json& j, Map& m)
{
    j.at("compressionlevel").get_to(m.compressionlevel);
    j.at("height").get_to(m.height);
    j.at("infinite").get_to(m.infinite);
    j.at("layers").get_to(m.layers);
    j.at("nextlayerid").get_to(m.nextlayerid);
    j.at("nextobjectid").get_to(m.nextobjectid);
    j.at("orientation").get_to(m.orientation);
    j.at("renderorder").get_to(m.renderorder);
    j.at("tiledversion").get_to(m.tiledversion);
    j.at("tileheight").get_to(m.tileheight);
    j.at("tilewidth").get_to(m.tilewidth);
    j.at("type").get_to(m.type);
    j.at("version").get_to(m.version);
    j.at("width").get_to(m.width);

    // Retrieve tileset name from tileset path
    for (const auto& [key, value] : j.at("tilesets").items())
    {
        long first_gid = value.at("firstgid").get<int>();
        std::string source = value.at("source").get<std::string>();
        m.tilesets.emplace(extractFileName(source, "/", "."), first_gid);
    }
}

Map parseMap(const std::string& path)
{
    try
    {
        nlohmann::json parsed_file;
        std::ifstream jsonFile(path);

        jsonFile >> parsed_file;
        Map map = parsed_file.get<Map>();

        return map;
    }
    catch (...)
    {
        // Some error handling idk.
        std::cerr << "Caught an unknown exception" << std::endl;
    }
}