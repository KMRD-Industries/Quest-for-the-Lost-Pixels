#pragma once

#include "Helpers.h"
#include "Tileset.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

void from_json(const json& j, ObjectProperty& p)
{
    j.at("name").get_to(p.name);
    j.at("type").get_to(p.type);
    j.at("value").get_to(p.value);
}

void from_json(const json& j, AnimationFrame& frame)
{
    j.at("duration").get_to(frame.duration);
    j.at("tileid").get_to(frame.tileid);
}

void from_json(const json& j, Collision& obj)
{
    j.at("id").get_to(obj.id);
    j.at("x").get_to(obj.x);
    j.at("y").get_to(obj.y);
    j.at("width").get_to(obj.width);
    j.at("height").get_to(obj.height);

    if (j.find("properties") != j.end())
    {
        j.at("properties").get_to(obj.properties);
    }
}

void from_json(const json& j, Tile& tile)
{
    j.at("id").get_to(tile.id);

    if (j.find("animation") != j.end())
    {
        j.at("animation").get_to(tile.animation);
    };

    if (j.find("objectgroup") != j.end())
    {
        auto objectgroup = j.at("objectgroup");

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

void from_json(const json& j, Tileset& tileset)
{
    j.at("columns").get_to(tileset.columns);
    j.at("imageheight").get_to(tileset.imageheight);
    j.at("imagewidth").get_to(tileset.imagewidth);
    j.at("margin").get_to(tileset.margin);
    j.at("name").get_to(tileset.name);
    j.at("spacing").get_to(tileset.spacing);
    j.at("tilecount").get_to(tileset.tilecount);
    j.at("tiledversion").get_to(tileset.tiledversion);
    j.at("tileheight").get_to(tileset.tileheight);
    j.at("tilewidth").get_to(tileset.tilewidth);
    j.at("type").get_to(tileset.type);
    j.at("version").get_to(tileset.version);

    if (j.find("tiles") != j.end())
    {
        j.at("tiles").get_to(tileset.tiles);
    }

    auto image = j.at("image");
    tileset.image = extractFileName(image.get<std::string>(), "/", ".");
}

Tileset parseTileset(const std::string& path)
{
    try
    {
        std::ifstream file(path);
        json j;
        file >> j;

        Tileset tileset = j.get<Tileset>();
        return tileset;
    }
    catch (...)
    {
        // Some error handling idk.
        std::cerr << "Caught an unknown exception" << std::endl;
    }
}