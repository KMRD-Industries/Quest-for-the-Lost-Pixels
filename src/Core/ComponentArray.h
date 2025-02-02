#pragma once

#include "IComponentArray.h"
#include "Types.h"

#include <array>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

template <typename T>
class ComponentArray final : public IComponentArray
{
public:
    void insertData(const Entity entity, const T &component)
    {
        if (m_entityToIndex.contains(entity))
        {
            throw std::logic_error("Component added to same entity more than once.");
        }

        const size_t newIndex{m_size};
        m_entityToIndex[entity] = newIndex;
        m_indexToEntity[newIndex] = entity;
        m_componentArray[newIndex] = component;
        ++m_size;
    }

    void removeData(const Entity entity)
    {
        if (!m_entityToIndex.contains(entity))
        {
            throw std::logic_error("Removing non-existent component.");
        }

        const size_t indexOfRemovedEntity{m_entityToIndex[entity]};
        const size_t indexOfLastElement{m_size - 1};
        m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

        const Entity entityOfLastElement{m_indexToEntity[indexOfLastElement]};
        m_entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
        m_indexToEntity[indexOfRemovedEntity] = entityOfLastElement;

        m_entityToIndex.erase(entity);
        m_indexToEntity.erase(indexOfLastElement);

        --m_size;
    }

    bool hasComponent(const Entity entity) const { return m_entityToIndex.contains(entity); }

    T *tryGetData(const Entity entity)
    {
        if (!m_entityToIndex.contains(entity)) return nullptr;
        return &m_componentArray[m_entityToIndex[entity]];
    }

    T &getData(const Entity entity)
    {
        if (!m_entityToIndex.contains(entity))
        {
            throw std::logic_error("Retrieving non-existent component.");
        }

        return m_componentArray[m_entityToIndex[entity]];
    }

    void entityDestroyed(const Entity entity) override
    {
        if (m_entityToIndex.contains(entity))
        {
            removeData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> m_componentArray{};
    std::unordered_map<Entity, size_t> m_entityToIndex{};
    std::unordered_map<size_t, Entity> m_indexToEntity{};
    size_t m_size{};
};
