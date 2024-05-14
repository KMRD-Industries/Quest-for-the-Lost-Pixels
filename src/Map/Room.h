#pragma once

#include <string>

class Room
{
public:
    Room() = default;
    Room(const int id) : m_mapID(id) {}
    void init();
    void draw() const;
    void update();
    std::string getMap() const;
    void setMapID(const int id) { m_mapID = id; }

private:
    int m_mapID{};
};
