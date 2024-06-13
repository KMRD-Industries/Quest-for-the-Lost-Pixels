#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "AnimationFrame.h"

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
    double width{};
    double height{};
    std::vector<ObjectProperty> properties;
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