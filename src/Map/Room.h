#pragma once

#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"

namespace GameType
{
    enum class DoorEntraces;
}
class Room
{
public:
    Room() = default;
    explicit Room(const int id) : m_mapID(id) {}
    void init();
    void draw() const;
    void update();
    [[nodiscard]] std::string getMap() const;
    void setMapID(const int id) { m_mapID = id; }

    std::unordered_map<GameType::DoorEntraces, glm::vec2> m_doorPosition;

private:
    int m_mapID{};
};
