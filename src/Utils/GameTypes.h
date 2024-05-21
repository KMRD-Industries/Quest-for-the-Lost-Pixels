#pragma once
#include <unordered_map>
#include <vector>
#include "Types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
namespace SpecialBlocks
{
    enum class Blocks : int
    {
        DOORSCOLLIDER = 0,
        STATICWALLCOLLIDER = 1
    };
} // namespace SpecialBlocks

namespace GameType
{
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
        int mapID{};
        std::vector<DoorEntraces> doorsLoc{};

        bool operator==(const MapInfo& other) const { return mapID == other.mapID; }
    };

    struct CollisionData
    {
        Entity entityID;
        std::string tag;
    };
} // namespace GameType

namespace std
{
    template <>
    struct hash<GameType::MapInfo>
    {
        size_t operator()(const GameType::MapInfo& mapInfo) const { return hash<int>()(mapInfo.mapID); }
    };
} // namespace std