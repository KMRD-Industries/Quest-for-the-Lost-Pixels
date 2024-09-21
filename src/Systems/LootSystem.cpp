#include "LootSystem.h"

#include <iostream>

void LootSystem::update()
{
    if (m_isCurrentRoomLooted)
        return;
    if (!m_entities.empty())
        m_toLoot = true;
    else if (m_entities.empty() && m_toLoot)
        spawnLoot();
}

void LootSystem::changeRoom(const glm::ivec2& newRoom)
{
    if (m_lootedRooms.contains(newRoom))
        m_isCurrentRoomLooted = m_lootedRooms.at(newRoom);
    else
    {
        m_lootedRooms.insert({newRoom, false});
        m_isCurrentRoomLooted = false;
    }
    m_currentRoom = newRoom;
    m_toLoot = false;
}

void LootSystem::spawnLoot()
{
    //Spawn logic
    std::cout << "[INFO] Spawning loot\n";

    m_lootedRooms[m_currentRoom] = true;
    m_isCurrentRoomLooted = true;
}