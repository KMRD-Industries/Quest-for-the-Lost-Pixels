#pragma once

#include <EnemySystem.h>
#include <SpawnerSystem.h>
#include <string>
#include <unordered_map>
#include <utility>

#include "DoorSystem.h"
#include "GameUtility.h"
#include "PassageSystem.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "glm/vec2.hpp"

namespace GameType
{
    enum class DoorEntraces;
}

class Room
{
public:
    Room() = default;

    explicit Room(std::string id, const int floor) : m_mapID(std::move(id)), m_floorID(floor) {}

    void init();
    void draw() const;
    void update();
    [[nodiscard]] std::string getMap() const;

    void setMapID(const std::string& mapID) { m_mapID = mapID; }
    int getFloorID() const;

    std::unordered_map<GameType::DoorEntraces, glm::vec2> m_doorPosition;

private:
    std::string m_mapID{};
    int m_floorID{0};
};
