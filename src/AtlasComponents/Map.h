#pragma once

#include <unordered_map>

struct Property
{
    std::string name;
    std::string type;
    std::string value;
};

struct Layer
{
    std::string compression;
    std::string data;
    std::string encoding;
    int height;
    int id;
    std::string name;
    float opacity;
    std::vector<Property> properties;
    std::string type;
    bool visible;
    int width;
    int x;
    int y;
};

struct Map
{
    int compressionlevel;
    int height;
    bool infinite;
    std::vector<Layer> layers;
    int nextlayerid;
    int nextobjectid;
    std::string orientation;
    std::string renderorder;
    std::string tiledversion;
    int tileheight;
    std::unordered_map<std::string, long> tilesets;
    int tilewidth;
    std::string type;
    std::string version;
    int width;
};