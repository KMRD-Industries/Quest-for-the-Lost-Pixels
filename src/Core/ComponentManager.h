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

        m_componentTypes.insert(std::make_pair(componentName, componentType));
        m_componentArrays.insert(std::make_pair(componentName, std::make_shared<ComponentArray<T>>()));
    }

    template <typename T>
    [[nodiscard]] auto getComponentType() const -> ComponentType
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
    auto getComponent(const Entity entity) const -> T&
    {
        return getComponentArray<T>()->getData(entity);
    }

    void entityDestroyed(Entity entity) const;

private:
    std::unordered_map<std::string, ComponentType> m_componentTypes{};
    std::unordered_map<std::string, std::shared_ptr<IComponentArray>> m_componentArrays{};
    ComponentType m_nextFreeComponentType{};

    template <typename T>
    auto getComponentArray() const -> std::shared_ptr<ComponentArray<T>>
    {
        const std::string componentName{typeid(T).name()};
        assert(m_componentTypes.contains(componentName) && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays.at(componentName));
    }
};
