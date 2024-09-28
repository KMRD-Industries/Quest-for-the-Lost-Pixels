#pragma once
#include <unordered_map>
#include <vector>
#include "Types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_math.h"
#include "glm/gtx/hash.hpp"

namespace MenuStateMachine
{
    enum class StateAction
    {
        Push,
        Pop
    };
}

namespace SpecialBlocks
{
    enum class Blocks : int
    {
        DOORSCOLLIDER = 0,
        STATICWALLCOLLIDER = 1,
        SPAWNERBLOCK = 2,
        BOSSSPAWNERBLOCK = 3,
        CHESTSPAWNERBLOCK = 4
    };
} // namespace SpecialBlocks

namespace Items
{
    enum class Behaviours : int
    {
        HEAL = 0,
        DMGUP = 1
    };
}

namespace Enemies
{
    enum class EnemyType : int
    {
        MELEE = 0,
        BOSS = 1
    };
} // namespace Enemies

namespace GameType
{
    struct MyVec2
    {
        float x, y;

        MyVec2(const float x, const float y) : x{x}, y{y}
        {
        }

        MyVec2(const glm::vec2& vec) : x(vec.x), y(vec.y)
        {
        }

        operator glm::vec2() const { return {x, y}; }

        MyVec2(const sf::Vector2f& vec) : x(vec.x), y(vec.y)
        {
        }

        operator sf::Vector2f() const { return {x, y}; }

        MyVec2(const b2Vec2& vec) : x(vec.x), y(vec.y)
        {
        }

        operator b2Vec2() const { return {x, y}; }
    };

    enum class DoorEntraces : int
    {
        NORTH = static_cast<int>('N'),
        SOUTH = static_cast<int>('S'),
        WEST = static_cast<int>('W'),
        EAST = static_cast<int>('E')
    };

    const std::unordered_map<DoorEntraces, glm::ivec2> mapDoorsToGeo{{DoorEntraces::NORTH, {0, 1}},
                                                                     {DoorEntraces::SOUTH, {0, -1}},
                                                                     {DoorEntraces::EAST, {1, 0}},
                                                                     {DoorEntraces::WEST, {-1, 0}}};
    const std::unordered_map<glm::ivec2, DoorEntraces> geoToMapDoors{{{0, 1}, DoorEntraces::NORTH},
                                                                     {{0, -1}, DoorEntraces::SOUTH},
                                                                     {{1, 0}, DoorEntraces::EAST},
                                                                     {{-1, 0}, DoorEntraces::WEST}};

    struct MapInfo
    {
        std::string mapID{};
        std::vector<DoorEntraces> doorsLoc{};

        bool operator==(const MapInfo& other) const { return mapID == other.mapID; }
    };

    struct CollisionData
    {
        Entity entityID;
        std::string tag;
    };

    struct RaycastData
    {
        Entity entityID;
        std::string tag;
        MyVec2 position;
    };
} // namespace GameType

namespace std
{
    template <>
    struct hash<GameType::MapInfo>
    {
        size_t operator()(const GameType::MapInfo& mapInfo) const { return hash<std::string>()(mapInfo.mapID); }
    };
} // namespace std