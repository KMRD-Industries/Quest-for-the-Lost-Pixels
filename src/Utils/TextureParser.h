#pragma once

#include <fstream>
#include "Helpers.h"
#include "Tileset.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void from_json(const json& json, ObjectProperty& objectProperty)
{
    json.at("name").get_to(objectProperty.name);
    json.at("type").get_to(objectProperty.type);
    json.at("value").get_to(objectProperty.value);
}

void from_json(const json& json, AnimationFrame& frame)
{
    json.at("duration").get_to(frame.duration);
    json.at("tileid").get_to(frame.tileid);
}

void from_json(const json& json, Collision& obj)
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

void from_json(const json& json, Tile& tile)
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

void from_json(const json& json, Tileset& tileset)
{
    json.at("columns").get_to(tileset.columns);
    json.at("imageheight").get_to(tileset.imageheight);
    json.at("imagewidth").get_to(tileset.imagewidth);
    json.at("margin").get_to(tileset.margin);
    json.at("name").get_to(tileset.name);
    json.at("spacing").get_to(tileset.spacing);
    json.at("tilecount").get_to(tileset.tilecount);
    json.at("tiledversion").get_to(tileset.tiledversion);
    json.at("tileheight").get_to(tileset.tileheight);
    json.at("tilewidth").get_to(tileset.tilewidth);
    json.at("type").get_to(tileset.type);
    json.at("version").get_to(tileset.version);

    if (json.find("tiles") != json.end())
    {
        json.at("tiles").get_to(tileset.tiles);
    }

    tileset.image = extractFileName(json.at("image").get<std::string>(), "/", ".");
}

Tileset parseTileset(const std::string& path)
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
