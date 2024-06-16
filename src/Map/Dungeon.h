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
    Dungeon() : m_entities(MAX_ENTITIES - 1) {}

    void init();
    void draw() const;
    void update();

private:
    void setECS();
    void makeSimpleFloor();
    void moveInDungeon(const glm::ivec2& dir);

    std::string m_asset_path{ASSET_PATH};
    FloorGenerator m_floorGenerator{};
    std::unordered_map<glm::ivec2, Room> m_roomMap{};
    glm::ivec2 m_currentPlayerPos{};
    std::vector<Entity> m_entities{};
    std::uint32_t m_id{};
    std::deque<glm::ivec2> m_moveInDungeon{};
    float counter = 0;
    bool m_passedBy = false;
};
