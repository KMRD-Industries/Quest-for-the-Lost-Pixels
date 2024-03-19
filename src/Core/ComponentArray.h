#pragma once

#include "Types.h"
#include "IComponentArray.h"

#include <array>
#include <cassert>
#include <unordered_map>

template<typename T>
class ComponentArray : public IComponentArray
{
public:
    void insertData(const Entity entity,const T& component)
    {
	assert(!m_entityToIndex.contains(entity) && "Component added to same entity more than once.");
	const size_t newIndex{m_size};
	m_entityToIndex[entity] = newIndex;
	m_indexToEntity[newIndex] = entity;
	m_componentArray[newIndex] = component;
	++m_size;
    }

    void removeData(const Entity entity)
    {
	assert(m_entityToIndex.contains(entity) && "Removing non-existent component.");

	const size_t indexOfRemovedEntity{m_entityToIndex[entity]};
	const size_t indexOfLastElement{m_size-1};
	m_componentArray[indexOfRemovedEntity] = m_componentArray[indexOfLastElement];

        const Entity entityOfLastElement{m_indexToEntity[indexOfLastElement]};
        m_entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
        m_indexToEntity[indexOfRemovedEntity] = entityOfLastElement;

        m_entityToIndex.erase(entity);
        m_indexToEntity.erase(indexOfLastElement);

        --m_size;
    }

    T& getData(const Entity entity)
    {
	assert(m_entityToIndex.contains(entity) && "Retrieving non-existent component.");
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
    std::array<T,MAX_ENTITIES> m_componentArray{};
    std::unordered_map<Entity, size_t> m_entityToIndex{};
    std::unordered_map<size_t, Entity> m_indexToEntity{};
    size_t m_size{};
};