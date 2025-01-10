#include "EntityManager.h"

#include <cassert>
#include <stdexcept>

EntityManager::EntityManager()
{
    for (Entity entity{}; entity < MAX_ENTITIES; ++entity)
    {
        m_availableEntities.push(entity);
    }
}

Entity EntityManager::createEntity()
{
    if (m_livingEntityCount >= MAX_ENTITIES)
    {
        throw std::logic_error("Too many entities in existence.");
    }

    const Entity id{m_availableEntities.front()};
    m_availableEntities.pop();
    ++m_livingEntityCount;

    return id;
}

void EntityManager::destroyEntity(const Entity entity)
{
    if (entity >= MAX_ENTITIES)
    {
        throw std::out_of_range("Entity out of range.");
    }

    m_signatures[entity].reset();
    m_availableEntities.push(entity);
    --m_livingEntityCount;
}

void EntityManager::setSignature(const Entity entity, const Signature& signature)
{
    if (entity >= MAX_ENTITIES)
    {
        throw std::out_of_range("Entity out of range.");
    }

    m_signatures[entity] = signature;
}

Signature EntityManager::getSignature(const Entity entity) const
{
    if (entity >= MAX_ENTITIES)
    {
        throw std::out_of_range("Entity out of range.");
    }

    return m_signatures[entity];
}
