#include "EntityManager.h"

#include <cassert>

EntityManager::EntityManager()
{
    for (Entity entity{}; entity < MAX_ENTITIES; ++entity)
    {
        m_availableEntities.push(entity);
    }
}

Entity EntityManager::createEntity()
{
    assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

    const Entity id{m_availableEntities.front()};
    m_availableEntities.pop();
    ++m_livingEntityCount;

    return id;
}

void EntityManager::destroyEntity(const Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    m_signatures[entity].reset();
    m_availableEntities.push(entity);
    --m_livingEntityCount;
}

void EntityManager::setSignature(const Entity entity, const Signature& signature)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    m_signatures[entity] = signature;
}

Signature EntityManager::getSignature(const Entity entity) const
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    return m_signatures[entity];
}
