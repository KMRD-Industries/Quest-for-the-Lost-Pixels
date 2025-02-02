#pragma once

#include "System.h"
#include "Types.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

class SystemManager
{
public:
    template <typename T>
    std::shared_ptr<T> registerSystem()
    {
        const std::string typeName{typeid(T).name()};

        if (m_systems.contains(typeName)) return std::static_pointer_cast<T>(m_systems[typeName]);

        const auto system{std::make_shared<T>()};
        m_systems[typeName] = std::static_pointer_cast<System>(system);
        return system;
    }

    template <typename T>
    void setSignature(const Signature& signature)
    {
        const std::string typeName{typeid(T).name()};

        if (!m_systems.contains(typeName))
        {
            throw std::logic_error("System used before registered.");
        }

        m_signatures[typeName] = signature;
    }

    void entityDestroyed(Entity entity);
    void entitySignatureChanged(Entity entity, const Signature& entitySignature);

private:
    std::unordered_map<std::string, Signature> m_signatures{};
    std::unordered_map<std::string, std::shared_ptr<System>> m_systems{};
};
