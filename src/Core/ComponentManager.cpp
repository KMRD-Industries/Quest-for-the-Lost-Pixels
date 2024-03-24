#include "ComponentManager.h"

void ComponentManager::entityDestroyed(const Entity entity) const
{
    for (const auto& component : m_componentArrays | std::views::values)
    {
        component->entityDestroyed(entity);
    }
}
