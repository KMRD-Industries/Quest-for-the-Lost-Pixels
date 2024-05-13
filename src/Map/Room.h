#pragma once

#include <string>

class Room
{
public:
    void init();
    void draw() const;
    void update();
    std::string getMap() const;
    void setMapID(const int id) { m_mapID = id; }

private:
    int m_mapID{};
};
