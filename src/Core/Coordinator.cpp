#include "Coordinator.h"

#include <iostream>
#include <ostream>

Coordinator::Coordinator(){};

void Coordinator::init()
{
    m_componentManager = std::make_unique<ComponentManager>();
    m_entityManager = std::make_unique<EntityManager>();
    m_systemManager = std::make_unique<SystemManager>();
    m_serverEntityManager = std::make_unique<ServerEntityManager>();
}

Entity Coordinator::createEntity() const { return m_entityManager->createEntity(); }
Entity Coordinator::createServerEntity() const { return m_serverEntityManager->createEntity(); }

void Coordinator::destroyEntity(const Entity entity) const
{
    m_entityManager->destroyEntity(entity);
    m_componentManager->entityDestroyed(entity);
    m_systemManager->entityDestroyed(entity);
    // TODO stworzyć ServerEntityManager i tutaj dodać, będzie służył tylko do mapowania wolnych encji z
    //  enetityMangaera do serverEntityManagera, który będzie przypisywał entity obiektom requestowanym do stworzenia
    //  przez server
}

void Coordinator::destroyServerEntity(const Entity entity) const { m_serverEntityManager->destroyEntity(entity); }

bool Coordinator::mapEntity(const Entity serverEntity, const Entity gameEntity) const
{
    return m_serverEntityManager->mapEntity(serverEntity, gameEntity);
}

Entity Coordinator::getServerEntity(const Entity gameEntity) const
{
    return m_serverEntityManager->getServerEntity(gameEntity);
}

Entity Coordinator::getGameEntity(const Entity serverEntity) const
{
    return m_serverEntityManager->getGameEntity(serverEntity);
}
