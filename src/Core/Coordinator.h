#pragma once

#include "ComponentManager.h"
#include "Core/Types.h"
#include "EntityManager.h"
#include "SystemManager.h"

#include <memory>

class Coordinator
{
public:
    Coordinator();
    void init();
    Entity createEntity() const;
    void destroyEntity(Entity entity) const;

    template <typename T>
    void registerComponent() const
    {
        m_componentManager->registerComponent<T>();
    }

    template <typename T>
    void addComponent(const Entity entity, const T& component)
    {
        m_componentManager->addComponent<T>(entity, component);

        auto signature{m_entityManager->getSignature(entity)};
        signature.set(m_componentManager->getComponentType<T>(), true);
        m_entityManager->setSignature(entity, signature);

        m_systemManager->entitySignatureChanged(entity, signature);
    }

    template <typename... Components>
    void addComponents(const Entity entity, const Components&... components)
    {
        (addComponent(entity, components), ...);
    }

    template <typename T>
    void removeComponent(const Entity entity)
    {
        m_componentManager->removeComponent<T>(entity);

        auto signature{m_entityManager->getSignature(entity)};
        signature.set(m_componentManager->getComponentType<T>(), false);
        m_entityManager->setSignature(entity, signature);

        m_systemManager->entitySignatureChanged(entity, signature);
    }

    template <typename T>
    T& getComponent(const Entity entity) const
    {
        return m_componentManager->getComponent<T>(entity);
    }

    template <typename T>
    bool hasComponent(const Entity entity) const
    {
        return m_componentManager->hasComponent<T>(entity);
    }

    template <typename T>
    ComponentType getComponentType() const
    {
        return m_componentManager->getComponentType<T>();
    }

    template <typename T>
    std::shared_ptr<T> getRegisterSystem()
    {
        return m_systemManager->registerSystem<T>();
    }

    template <typename T>
    void setSystemSignature(const Signature signature) const
    {
        m_systemManager->setSignature<T>(signature);
    }

private:
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<SystemManager> m_systemManager;
};
