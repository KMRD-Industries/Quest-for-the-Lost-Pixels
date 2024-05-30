#pragma once

#include "System.h"
#include "Types.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

class SystemManager
{
public:
    template <typename T>
    std::shared_ptr<T> registerSystem()
    {
        const std::string typeName{typeid(T).name()};

        if (m_systems.contains(typeName)) return std::static_pointer_cast<T>(m_systems.at(typeName));

        const auto system{std::make_shared<T>()};

        m_systems.insert(std::make_pair(typeName, std::static_pointer_cast<System>(system)));
        return system;
    }

    template <typename T>
    void setSignature(const Signature& signature)
    {
        const std::string typeName{typeid(T).name()};

        assert(m_systems.contains(typeName) && "System used before registered.");

        m_signatures.insert(std::make_pair(typeName, signature));
    }

    void entityDestroyed(Entity entity);
    void entitySignatureChanged(Entity entity, const Signature& entitySignature);

private:
    std::unordered_map<std::string, Signature> m_signatures{};
    std::unordered_map<std::string, std::shared_ptr<System>> m_systems{};
};
