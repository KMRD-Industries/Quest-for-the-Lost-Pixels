#pragma once
#include "Coordinator.h"
#include "System.h"

struct TransformComponent;
extern Coordinator gCoordinator;

class LootSpawnerSystem : public System
{
public:
    void spawnChest() const;

private:
    void clearSpawners() const;
    void processSpawn(const TransformComponent& spawnerTransformComponent) const;
};