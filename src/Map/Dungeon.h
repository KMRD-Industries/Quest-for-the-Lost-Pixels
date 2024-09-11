#pragma once
#include <deque>
#include <unordered_map>
#include <vector>

#include "FloorGenerator.h"
#include "Paths.h"
#include "Room.h"
#include "Types.h"

class Dungeon
{
public:
    Dungeon() = default;

    static void init();
    static void draw();
    static void update();

private:
    static void setECS();
    static void makeSimpleFloor();
    static void moveInDungeon(const glm::ivec2& dir);
    static void clearDungeon();

    static std::string m_asset_path;
    static FloorGenerator m_floorGenerator;
    static std::unordered_map<glm::ivec2, Room> m_roomMap;
    static glm::ivec2 m_currentPlayerPos;
    static std::vector<Entity> m_entities;
    static std::uint32_t m_id;
    static std::deque<glm::ivec2> m_moveInDungeon;
    static float counter;
    static bool m_passedBy;
};
