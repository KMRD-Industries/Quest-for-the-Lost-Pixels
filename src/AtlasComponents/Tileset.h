#pragma once

#include <string>
#include <vector>
#include "AnimationFrame.h"
#include "Config.h"

struct ObjectProperty
{
    std::string name;
    std::string type;
    std::string value;
};

struct Collision
{
    int id{};
    double x{};
    double y{};
    double width{config::tileHeight};
    double height{config::tileHeight};
    std::vector<ObjectProperty> properties;

    // Equality operator
    bool operator==(const Collision& other) const
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
};

struct Tile
{
    int id;
    std::vector<AnimationFrame> animation;
    std::vector<Collision> objects;
};

struct Tileset
{
    int columns;
    std::string image;
    int imageheight;
    int imagewidth;
    int margin;
    std::string name;
    int spacing;
    int tilecount;
    std::string tiledversion;
    int tileheight;
    std::vector<Tile> tiles;
    int tilewidth;
    std::string type;
    std::string version;
};
