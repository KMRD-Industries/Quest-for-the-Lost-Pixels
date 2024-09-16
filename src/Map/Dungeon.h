#pragma once
#include <deque>
#include <unordered_map>
#include <set>
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
    static void createRemotePlayer(uint32_t id);
    static void moveInDungeon(const glm::ivec2& dir);
    static void clearDungeon();
	static float getSpawnOffset(float position, int id);

    static std::string m_asset_path{ASSET_PATH};
    static FloorGenerator m_floorGenerator{};
    static std::unordered_map<glm::ivec2, Room> m_roomMap{};
    static glm::ivec2 m_currentPlayerPos{};
    static std::vector<Entity> m_entities{};
    static std::set<uint32_t> m_players{};
    static uint32_t m_id{};
    static int64_t m_seed{};
    static std::deque<glm::ivec2> m_moveInDungeon{};
    static float counter = 0;
    static bool m_passedBy = false;
};
