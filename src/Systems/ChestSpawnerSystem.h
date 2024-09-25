#pragma once
#include "Coordinator.h"
#include "System.h"
#include "Tileset.h"

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
    void spawnWeapon(const TransformComponent& spawnerTransformComponent);
    ChestSpawnerSystem();
    void init();

private:
    void clearSpawners() const;
    void processSpawn(const TransformComponent&);
    void handleChestCollision(Entity chest, const TransformComponent&, const GameType::CollisionData&);
    void spawnPotion(const TransformComponent&);

    std::vector<AnimationData> m_itemsToAnimate;
    std::vector<int> m_weaponsIDs;
    Collision m_chestCollision;
    Collision m_potionCollision;
};