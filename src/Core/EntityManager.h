#pragma once

#include <array>
#include <queue>
#include "Types.h"

class EntityManager
{
public:
    EntityManager();
    Entity createEntity();
    void destroyEntity(Entity entity);
    void setSignature(Entity entity, const Signature& signature);
    [[nodiscard]] Signature getSignature(Entity entity) const;

private:
    std::queue<Entity> m_availableEntities{};
    std::array<Signature, MAX_ENTITIES> m_signatures{};
    uint32_t m_livingEntityCount{};
};
