#include "Coordinator.h"

Coordinator::Coordinator() = default;

void Coordinator::init()
{
    m_componentManager = std::make_unique<ComponentManager>();
    m_entityManager = std::make_unique<EntityManager>();
    m_systemManager = std::make_unique<SystemManager>();
}

auto Coordinator::createEntity() const -> Entity { return m_entityManager->createEntity(); }

[[maybe_unused]] void Coordinator::destroyEntity(const Entity entity) const
{
    m_entityManager->destroyEntity(entity);
    m_componentManager->entityDestroyed(entity);
    m_systemManager->entityDestroyed(entity);
}
