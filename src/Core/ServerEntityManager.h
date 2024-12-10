#pragma once

#include <queue>
#include <unordered_map>
#include "Types.h"

class ServerEntityManager
{
public:
    ServerEntityManager();
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool mapEntity(const Entity serverEntity, Entity gameEntity);
    Entity getServerEntity(const Entity gameEntity) const;
    Entity getGameEntity(const Entity serverEntity) const;

private:
    std::unordered_map<Entity, Entity> m_serverEntityToGameEntity;
    std::unordered_map<Entity, Entity> m_gameEntityToServerEntity;
    std::queue<Entity> m_availableServerEntities;
    uint32_t m_livingEntityCount{};
};
