#pragma once

#include <array>
#include <queue>
#include "Types.h"

class EntityManager
{
public:
    EntityManager();
    auto createEntity() -> Entity;
    void destroyEntity(Entity entity);
    void setSignature(Entity entity, const Signature& signature);
    [[nodiscard]] auto getSignature(Entity entity) const -> Signature;

private:
    std::queue<Entity> m_availableEntities{};
    std::array<Signature, MAX_ENTITIES> m_signatures{};
    uint32_t m_livingEntityCount{};
};
