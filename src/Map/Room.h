#pragma once

#include <string>
#include <unordered_map>

#include "GameUtility.h"
#include "glm/vec2.hpp"

namespace GameType
{
    enum class DoorEntraces;
}

class Room
{
public:
    Room() = default;

    explicit Room(const int id) :
        m_mapID(id)
    {
    }

    void init();
    void draw() const;
    void update();
    [[nodiscard]] std::string getMap() const;
    void setMapID(const int id) { m_mapID = id; }
    void setFloorID(const int id){m_floorID = id;};
    int getFloorID() const;

    std::unordered_map<GameType::DoorEntraces, glm::vec2> m_doorPosition;

private:
    int m_mapID{0};
    int m_floorID{0};
};
