#pragma once
#include "Coordinator.h"
#include "GameTypes.h"
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
    ChestSpawnerSystem();
    void spawnChest() const;

private:
    void clearSpawners() const;
    void spawnItem(const TransformComponent& spawnerTransformComponent) const;
    void processSpawn(const TransformComponent&) const;
    void handleChestCollision(Entity chest, const GameType::CollisionData&) const;

    std::vector<AnimationData> m_itemsToAnimate;
    std::vector<std::pair<uint32_t, GameType::WeaponType>> m_weaponsIDs;
    std::vector<uint32_t> m_helmetsIDs;
    std::vector<uint32_t> m_bodyArmoursIDs;
    Collision m_chestCollision;
    Collision m_potionCollision;
    TileComponent m_chestTile;
};
