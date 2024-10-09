#pragma once
#include "Config.h"
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
    void spawnChest() const;
    ChestSpawnerSystem();
    void init();

private:
    void clearSpawners() const;
    void spawnItem(const TransformComponent &spawnerTransformComponent, config::itemLootType) const;
    void processSpawn(const TransformComponent &) const;
    void handleChestCollision(Entity chest, const GameType::CollisionData &) const;
    void spawnItem(const TransformComponent &) const;

    std::vector<AnimationData> m_itemsToAnimate;
    std::vector<std::pair<int, GameType::WeaponType> > m_weaponsIDs;
    std::vector<int> m_helmetsIDs;
    std::vector<int> m_bodyArmoursIDs;

    Collision m_chestCollision;
    Collision m_potionCollision;
    TileComponent m_chestTile;
};
