#include "Coordinator.h"

Coordinator::Coordinator() {  };

void Coordinator::init()
{
    m_componentManager = std::make_unique<ComponentManager>();
    m_entityManager = std::make_unique<EntityManager>();
    m_systemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::createEntity() const
{
    return m_entityManager->createEntity();
}

void Coordinator::destroyEntity(const Entity entity) const
{
    m_entityManager->destroyEntity(entity);
    m_componentManager->entityDestroyed(entity);
    m_systemManager->entityDestroyed(entity);
}
