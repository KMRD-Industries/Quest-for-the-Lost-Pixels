#include "RoomListenerSystem.h"

#include <SpawnerSystem.h>
#include <iostream>

#include "ChestSpawnerSystem.h"
#include "PassageSystem.h"

void RoomListenerSystem::update(const float deltaTime)
{
    if (m_isCurrentRoomLooted)
        return;
    if (!m_entities.empty())
        m_toLoot = true;
    else if (m_entities.empty() && m_toLoot)
        spawnLoot();
    else
        gCoordinator.getRegisterSystem<SpawnerSystem>()->update(deltaTime);
}

void RoomListenerSystem::reset()
{
    m_isCurrentRoomLooted = true;
    m_isCurrentRoomClear = false;
    m_lootedRooms.clear();
    m_toLoot = false;
}

void RoomListenerSystem::changeRoom(const glm::ivec2& newRoom)
{
    if (m_lootedRooms.contains(newRoom))
    {
        m_isCurrentRoomLooted = m_lootedRooms.at(newRoom);
        m_isCurrentRoomClear = !m_isCurrentRoomLooted;
    }
    else
    {
        m_lootedRooms.insert({newRoom, false});
        m_isCurrentRoomLooted = false;
        gCoordinator.getRegisterSystem<SpawnerSystem>()->spawnEnemies();
    }
    m_currentRoom = newRoom;
    m_toLoot = false;
}

void RoomListenerSystem::spawnLoot()
{
    gCoordinator.getRegisterSystem<ChestSpawnerSystem>()->spawnChest();
    gCoordinator.getRegisterSystem<PassageSystem>()->setPassages(true);
    std::cout << "[INFO] Spawning room content (chests, portals)\n";

    m_lootedRooms[m_currentRoom] = true;
    m_isCurrentRoomLooted = true;
}