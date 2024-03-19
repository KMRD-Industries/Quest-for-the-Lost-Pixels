#pragma once

#include "Types.h"
#include "System.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <cassert>

class SystemManager
{
public:
    template <typename T>
    std::shared_ptr<T> registerSystem()
    {
        const std::string typeName{typeid(T).name()};

        assert(!m_systems.contains(typeName) && "Registering system more than once.");

        const auto system{std::make_shared<T>()};
        m_systems[typeName] = std::static_pointer_cast<System>(system);
        return system;
    }

    template <typename T>
    void setSignature(const Signature& signature)
    {
        const std::string typeName{typeid(T).name()};

        assert(m_systems.contains(typeName) && "System used before registered.");

        m_signatures[typeName] = signature;
    }

    void entityDestroyed(const Entity entity);
    void entitySignatureChanged(const Entity entity, const Signature& entitySignature);

private:
    std::unordered_map<std::string, Signature> m_signatures{};
    std::unordered_map<std::string, std::shared_ptr<System>> m_systems{};
};
