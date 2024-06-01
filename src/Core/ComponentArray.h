#pragma once

#include "IComponentArray.h"
#include "Types.h"

#include <array>
#include <cassert>
#include <unordered_map>

template <typename T>
class ComponentArray final : public IComponentArray
{
public:
    void insertData(const Entity entity, const T& component)
    {
        assert(!m_entityToIndex.contains(entity) && "Component added to same entity more than once.");
        const size_t newIndex{m_size};

        m_entityToIndex.insert(std::make_pair(entity, newIndex));
        m_indexToEntity.emplace(newIndex, entity).first->second = entity;

        m_componentArray[newIndex] = component;
        ++m_size;
    }

    void removeData(const Entity entity)
    {
        assert(m_entityToIndex.contains(entity) && "Removing non-existent component.");

        const size_t indexOfRemovedEntity{m_entityToIndex.at(entity)};
        const size_t indexOfLastElement{m_size - 1};
        m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

        const Entity entityOfLastElement{m_indexToEntity.at(indexOfLastElement)};

        m_entityToIndex.emplace(entityOfLastElement, indexOfRemovedEntity).first->second = indexOfRemovedEntity;
        m_indexToEntity.emplace(indexOfRemovedEntity, entityOfLastElement).first->second = entityOfLastElement;

        m_entityToIndex.erase(entity);
        m_indexToEntity.erase(indexOfLastElement);

        --m_size;
    }

    auto getData(const Entity entity) -> T&
    {
        assert(m_entityToIndex.contains(entity) && "Retrieving non-existent component.");
        return m_componentArray.at(m_entityToIndex.at(entity));
    }

    void entityDestroyed(const Entity entity) override
    {
        if (m_entityToIndex.contains(entity))
        {
            removeData(entity);
        }
    }

    auto hasData(const Entity entity) const -> bool { return m_entityToIndex.contains(entity); }

private:
    std::array<T, MAX_ENTITIES> m_componentArray{};
    std::unordered_map<Entity, size_t> m_entityToIndex{};
    std::unordered_map<size_t, Entity> m_indexToEntity{};
    size_t m_size{};
};
