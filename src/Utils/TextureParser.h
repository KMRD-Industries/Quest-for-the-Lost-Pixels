#pragma once

#include <fstream>
#include "Helpers.h"
#include "Tileset.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

inline void from_json(const json& json, ObjectProperty& objectProperty)
{
    json.at("name").get_to(objectProperty.name);
    json.at("type").get_to(objectProperty.type);
    json.at("value").get_to(objectProperty.value);
}

inline void from_json(const json& json, AnimationFrame& frame)
{
    json.at("duration").get_to(frame.duration);
    json.at("tileid").get_to(frame.tileID);
}

inline void from_json(const json& json, Collision& obj)
{
    json.at("id").get_to(obj.id);
    json.at("x").get_to(obj.x);
    json.at("y").get_to(obj.y);
    json.at("width").get_to(obj.width);
    json.at("height").get_to(obj.height);

    if (json.find("properties") != json.end())
    {
        json.at("properties").get_to(obj.properties);
    }
}

inline void from_json(const json& json, Tile& tile)
{
    json.at("id").get_to(tile.id);

    if (json.find("animation") != json.end())
    {
        json.at("animation").get_to(tile.animation);
    };

    if (json.find("objectgroup") != json.end())
    {
        auto objectgroup = json.at("objectgroup");

        if (objectgroup.find("animations") != objectgroup.end())
        {
            objectgroup.at("animations").get_to(tile.animation);
        }

        if (objectgroup.find("objects") != objectgroup.end())
        {
            objectgroup.at("objects").get_to(tile.objects);
        }
    };
}

inline void from_json(const json& json, Tileset& tileSet)
{
    json.at("columns").get_to(tileSet.columns);
    json.at("imageheight").get_to(tileSet.imageheight);
    json.at("imagewidth").get_to(tileSet.imagewidth);
    json.at("margin").get_to(tileSet.margin);
    json.at("name").get_to(tileSet.name);
    json.at("spacing").get_to(tileSet.spacing);
    json.at("tilecount").get_to(tileSet.tilecount);
    json.at("tiledversion").get_to(tileSet.tiledversion);
    json.at("tileheight").get_to(tileSet.tileheight);
    json.at("tilewidth").get_to(tileSet.tilewidth);
    json.at("type").get_to(tileSet.type);
    json.at("version").get_to(tileSet.version);

    if (json.find("tiles") != json.end())
    {
        json.at("tiles").get_to(tileSet.tiles);
    }

    tileSet.image = extractFileName(json.at("image").get<std::string>(), "/", ".");
}

inline Tileset parseTileSet(const std::string& path)
{
    try
    {
        std::ifstream file(path);
        json j;
        file >> j;

        return j.get<Tileset>();
    }
    catch (...)
    {
        std::cerr << "Caught an unknown exception" << std::endl;
        return {};
    }
}
