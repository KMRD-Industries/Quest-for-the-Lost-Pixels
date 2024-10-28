#include "ServerEntityManager.h"

#include <cassert>
#include <iostream>

ServerEntityManager::ServerEntityManager()
{
    for (Entity entity{}; entity < MAX_ENTITIES; ++entity)
    {
        m_availableServerEntities.push(entity);
    }
}

Entity ServerEntityManager::createEntity()
{
    assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

    const Entity id{m_availableServerEntities.front()};
    m_availableServerEntities.pop();
    ++m_livingEntityCount;

    return id;
}

void ServerEntityManager::destroyEntity(const Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    m_gameEntityToServerEntity.erase(m_serverEntityToGameEntity[entity]);
    m_serverEntityToGameEntity.erase(entity);
    m_availableServerEntities.push(entity);
    --m_livingEntityCount;
}

void ServerEntityManager::mapEntity(Entity serverEntity, Entity gameEntity)
{
    if (const auto [fst, snd] = m_serverEntityToGameEntity.emplace(serverEntity, gameEntity); !snd)
    {
        std::cerr << "ServerEntityManager::mapEntity: Entity already exists.\n";
    }

    if (const auto [fst, snd] = m_gameEntityToServerEntity.emplace(gameEntity, serverEntity); !snd)
    {
        std::cerr << "ServerEntityManager::mapEntity: Entity already exists.\n";
    }
}

Entity ServerEntityManager::getServerEntity(const Entity gameEntity) const
{
    if (const auto it = m_gameEntityToServerEntity.find(gameEntity); it != m_gameEntityToServerEntity.end())
    {
        return it->second;
    }
    return 0;
}

Entity ServerEntityManager::getGameEntity(const Entity serverEntity) const
{
    if (const auto it = m_serverEntityToGameEntity.find(serverEntity); it != m_serverEntityToGameEntity.end())
    {
        return it->second;
    }
    return 0;
}

// TODO podział na mapę graczy/oponentów oddzielnie
