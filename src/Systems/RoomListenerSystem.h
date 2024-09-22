#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include "Coordinator.h"
#include "System.h"
#include "glm/vec2.hpp"

extern Coordinator gCoordinator;

class RoomListenerSystem : public System
{
public:
    void update();
    void changeRoom(const glm::ivec2& newRoom);
    void spawnLoot();

private:
    std::unordered_map<glm::ivec2, bool> m_lootedRooms{};
    bool m_isCurrentRoomLooted{};
    bool m_toLoot{};
    glm::ivec2 m_currentRoom{};
};