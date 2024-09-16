#include "Room.h"

#include <string>

#include "Paths.h"

void Room::init()
{
}

void Room::draw() const
{
}

void Room::update()
{
}

std::string Room::getMap() const
{
    const std::string path{"/maps/map_"};
    const std::string suffix{".json"};
    return std::string(ASSET_PATH) + path + m_mapID + suffix;
}