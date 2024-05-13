#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <unordered_map>
#include <vector>

#include "FloorGenerator.h"
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

    FloorGenerator m_floorGenerator{};
    std::unordered_map<glm::ivec2, Room> m_roomMap{};
    glm::ivec2 m_currentPlayerPos{};
    std::vector<Entity> m_entities{};
};
