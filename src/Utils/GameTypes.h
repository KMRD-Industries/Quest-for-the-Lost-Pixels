#pragma once
#include <vector>
#include "Types.h"

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