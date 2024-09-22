#pragma once
#include "Coordinator.h"
#include "System.h"

struct ColliderComponent;

namespace GameType
{
    struct CollisionData;
}

struct TransformComponent;
extern Coordinator gCoordinator;

struct AnimationData
{
    float animationDuration{};
    float currentAnimationTime{};
    Entity entityToAnimate{};
    bool destroy{};
    float startingPositionY{};
    bool shouldAnimate{true};
};

class ChestSpawnerSystem : public System
{
public:
    void spawnChest();

private:
    void clearSpawners() const;
    void processSpawn(const TransformComponent& spawnerTransformComponent);
    void handleChestCollision(Entity chest, const TransformComponent& spawnerTransformComponent,
                              const GameType::CollisionData& collisionData);
    void spawnPotion(const TransformComponent& spawnerTransformComponent);

    std::vector<AnimationData> m_itemsToAnimate;
};