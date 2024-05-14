#include "FloorGenerator.h"

#include <algorithm>
#include <ranges>

#include "Coordinator.h"
#include "MapSystem.h"

extern Coordinator gCoordinator;


void FloorGenerator::generateFloor(const int h, const int w) { m_generator = DungeonGenerator(h, w); }

std::unordered_map<glm::ivec2, Room> FloorGenerator::getFloor(const bool generate)
{
    if (!generate) return m_floorMap;

    m_floorMap.clear();

    const auto avaibleMaps = gCoordinator.getRegisterSystem<MapSystem>()->getMapInfo();

    std::unordered_map<GameType::MapInfo, int> choosedMap{};

    const auto graphMap = m_generator.getGraph();

    for (const auto& [nodePosition, nodeNeighbours] : graphMap)
    {
        std::unordered_set<GameType::DoorEntraces> doorsForRoom;
        for (const auto& neighbour : nodeNeighbours)
        {
            const auto dir = neighbour - nodePosition;
            if (m_mapDirOnGraphToEntrace.contains(dir)) doorsForRoom.insert(m_mapDirOnGraphToEntrace.at(dir));
        }
        std::vector<GameType::MapInfo> avaibleMapsForRoom;
        const auto haveSameDoorsPlacement = [&doorsForRoom](GameType::MapInfo mapInfo)
        {
            const std::unordered_set<GameType::DoorEntraces> mapDoors(mapInfo.doorsLoc.begin(), mapInfo.doorsLoc.end());
            return mapDoors == doorsForRoom;
        };
        std::ranges::copy_if(avaibleMaps, std::back_inserter(avaibleMapsForRoom), haveSameDoorsPlacement);

        int minVal = std::numeric_limits<int>::max();

        for (const auto& mapInfo : avaibleMapsForRoom)
        {
            minVal = choosedMap[mapInfo];
        }

        std::vector<GameType::MapInfo> mapToChoose;
        const auto haveMinimalValueOfDoors = [&choosedMap, &minVal](GameType::MapInfo mapInfo)
        { return choosedMap[mapInfo] == minVal; };
        std::ranges::copy_if(avaibleMapsForRoom, std::back_inserter(mapToChoose), haveMinimalValueOfDoors);

        std::random_device rd;
        std::mt19937 g(rd());

        std::ranges::shuffle(mapToChoose, g);

        auto selectedMap = mapToChoose.front();
        choosedMap[selectedMap]++;
        m_floorMap[nodePosition] = Room(selectedMap.mapID);
    }

    return m_floorMap;
}
