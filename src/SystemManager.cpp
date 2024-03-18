#include "SystemManager.h"

#include <ranges>

#include "System.h"

void SystemManager::entityDestroyed(const Entity entity)
{
    for (const auto& system : m_systems | std::views::values) 
    {
        system->m_entities.erase(entity);
    }
}

void SystemManager::entitySignatureChanged(const Entity entity,const Signature& entitySignature)
{
    for (const auto& [type, system] : m_systems) 
    {
        const auto& systemSignature{m_signatures[type]};

        if((entitySignature & systemSignature) == systemSignature)
        {
            system->m_entities.insert(entity);
        }
        else
        {
            system->m_entities.erase(entity);
        }
    }
}