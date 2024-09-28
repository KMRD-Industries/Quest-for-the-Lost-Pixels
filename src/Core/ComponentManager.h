#pragma once

#include "ComponentArray.h"
#include "Types.h"

#include <memory>
#include <ranges>
#include <unordered_map>

class IComponentArray;

class ComponentManager
{
public:
    template <typename T>
    void registerComponent()
    {
        const std::string componentName{typeid(T).name()};

        assert(!m_componentTypes.contains(componentName) && "Registering component type more than once.");

        const ComponentType componentType{m_nextFreeComponentType};
        ++m_nextFreeComponentType;

        m_componentTypes[componentName] = componentType;
        m_componentArrays[componentName] = std::make_shared<ComponentArray<T>>();
    }

    template <typename T>
    [[nodiscard]] ComponentType getComponentType() const
    {
        const std::string componentName{typeid(T).name()};

        assert(m_componentTypes.contains(componentName) && "Component not registered before use.");

        return m_componentTypes.at(componentName);
    }

    template <typename T>
    void addComponent(const Entity entity, const T component) const
    {
        getComponentArray<T>()->insertData(entity, component);
    }

    template <typename T>
    void removeComponent(const Entity entity) const
    {
        getComponentArray<T>()->removeData(entity);
    }

    template <typename T>
    T& getComponent(const Entity entity) const
    {
        return getComponentArray<T>()->getData(entity);
    }

    template <typename T>
    T* tryGetComponent(const Entity entity) const
    {
        return getComponentArray<T>()->tryGetData(entity);
    }

    template <typename T>
    [[nodiscard]] bool hasComponent(const Entity entity) const
    {
        return getComponentArray<T>()->hasComponent(entity);
    }

    void entityDestroyed(Entity entity) const;

private:
    std::unordered_map<std::string, ComponentType> m_componentTypes{};
    std::unordered_map<std::string, std::shared_ptr<IComponentArray>> m_componentArrays{};
    ComponentType m_nextFreeComponentType{};

    template <typename T>
    std::shared_ptr<ComponentArray<T>> getComponentArray() const
    {
        const std::string componentName{typeid(T).name()};

        assert(m_componentTypes.contains(componentName) && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays.at(componentName));
    }
};
