#pragma once
#include "Coordinator.h"
#include "System.h"
#include "TileComponent.h"
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
    void spawnWeapon(const TransformComponent& spawnerTransformComponent) const;
    ChestSpawnerSystem();
    void init();

private:
    void clearSpawners() const;
    void processSpawn(const TransformComponent&) const;
    void handleChestCollision(Entity chest, const GameType::CollisionData&) const;
    void spawnPotion(const TransformComponent&) const;

    std::vector<AnimationData> m_itemsToAnimate;
    std::vector<int> m_weaponsIDs;
    Collision m_chestCollision;
    Collision m_potionCollision;
    TileComponent m_chestTile;
};