#include "FloorGenerator.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>

#include "AnimationSystem.h"
#include "Utils/Helpers.h"
#include "Utils/Paths.h"

void FloorGenerator::generateFloor(const int h, const int w) { m_generator = DungeonGenerator(h, w); }

bool FloorGenerator::isConnected(const glm::ivec2& firstNode, const glm::ivec2& secondNode) const
{
    return m_generator.isConnected(firstNode, secondNode);
}

std::unordered_map<glm::ivec2, Room> FloorGenerator::getFloor(const bool generate)
{
    const auto firstRoom = getStartingRoom();

    if (!generate)
        return m_floorMap;

    m_floorMap.clear();

    const auto availableMaps = getMapInfo();

    std::unordered_map<GameType::MapInfo, int> choosesMap{};

    const auto graphMap = m_generator.getGraph();

    for (const auto& [nodePosition, nodeNeighbors] : graphMap)
    {
        std::unordered_set<GameType::DoorEntraces> doorsForRoom;
        for (const auto& neighbor : nodeNeighbors)
            if (const auto dir = neighbor - nodePosition; m_mapDirOnGraphToEntrance.contains(dir))
                doorsForRoom.insert(m_mapDirOnGraphToEntrance.at(dir));

        std::vector<GameType::MapInfo> availableMapsForRoom;
        const auto haveSameDoorsPlacement = [&doorsForRoom, &nodePosition, &firstRoom](const GameType::MapInfo& mapInfo)
        {
            bool isFirstRoomCorrect = true;
            const std::unordered_set<GameType::DoorEntraces> mapDoors(mapInfo.doorsLoc.begin(), mapInfo.doorsLoc.end());
            if (nodePosition == firstRoom)
            {
                if (mapInfo.mapID[0] != 's')
                    isFirstRoomCorrect = false;
            }
            else
            {
                if (mapInfo.mapID[0] == 's')
                    isFirstRoomCorrect = false;
            }
            return mapDoors == doorsForRoom && isFirstRoomCorrect;
        };
        std::ranges::copy_if(availableMaps, std::back_inserter(availableMapsForRoom), haveSameDoorsPlacement);

        int minVal = std::numeric_limits<int>::max();

        for (const auto& mapInfo : availableMapsForRoom)
            minVal = choosesMap[mapInfo];

        std::vector<GameType::MapInfo> mapToChoose;
        const auto haveMinimalValueOfDoors = [&choosesMap, &minVal](const GameType::MapInfo& mapInfo)
        {
            return choosesMap[mapInfo] == minVal;
        };

        std::ranges::copy_if(availableMapsForRoom, std::back_inserter(mapToChoose), haveMinimalValueOfDoors);

        std::random_device rd;
        std::mt19937 g(rd());

        std::ranges::shuffle(mapToChoose, g);

        auto selectedMap = mapToChoose.front();
        choosesMap[selectedMap]++;
        m_floorMap[nodePosition] = Room(selectedMap.mapID, m_floorID);
    }

    return m_floorMap;
}

std::vector<GameType::MapInfo> FloorGenerator::getMapInfo() const
{
    namespace fs = std::filesystem;
    const std::string path = std::string(ASSET_PATH) + "/maps/floor_0" + std::to_string(m_floorID) + "/";

    std::vector<GameType::MapInfo> mapInfo{};

    try
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            std::cerr << "Directory does not exist or is not a directory: " << path << std::endl;
            return mapInfo;
        }

        for (const auto& entry : fs::directory_iterator(path))
            checkSingleFile(entry, mapInfo);
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return mapInfo;
    }
    catch (const std::regex_error& e)
    {
        std::cerr << "Regex error: " << e.what() << std::endl;
        return mapInfo;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return mapInfo;
    }

    return mapInfo;
}

void FloorGenerator::checkSingleFile(const std::filesystem::directory_entry& entry,
                                     std::vector<GameType::MapInfo>& mapInfo)
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    if (!entry.is_regular_file())
        return;

    const std::regex pattern(R"(map_.*\.json)");
    const std::string filename = entry.path().filename().string();
    const size_t underscorePos = filename.find_last_of('_');
    const size_t dotPos = filename.find_last_of('.');
    const std::string mapID = filename.substr(underscorePos + 1, dotPos - underscorePos - 1);
    const std::string numberStr = filename.substr(underscorePos + 1, dotPos - underscorePos - 1);
    // const int mapID = std::stoi(numberStr);
    // const int mapID = 110;

    if (!std::regex_match(filename, pattern))
        return;

    std::cout << "Found matching file: " << filename << std::endl;

    std::ifstream file(entry.path());
    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    json parsed_file;
    file >> parsed_file;

    const auto& doorData = findSpecialBlocks(parsed_file);

    const int mapWidth = parsed_file["width"];
    const int mapHeight = parsed_file["height"];

    std::vector<glm::ivec2> doorsPositions;

    for (const auto& [doorPosition, blockType] : doorData)
    {
        if (blockType != static_cast<int>(SpecialBlocks::Blocks::DOORSCOLLIDER))
            continue;
        doorsPositions.push_back(doorPosition);
    }

    std::unordered_set<GameType::DoorEntraces> doorsLoc;

    for (const auto& doorPosition : doorsPositions)
    {
        if (doorPosition.y == 0)
            doorsLoc.insert(GameType::DoorEntraces::NORTH);
        else if (doorPosition.y == mapHeight - 1)
            doorsLoc.insert(GameType::DoorEntraces::SOUTH);

        if (doorPosition.x == 0)
            doorsLoc.insert(GameType::DoorEntraces::WEST);
        else if (doorPosition.x == mapWidth - 1)
            doorsLoc.insert(GameType::DoorEntraces::EAST);
    }

    mapInfo.emplace_back(GameType::MapInfo{.mapID = mapID, .doorsLoc = {doorsLoc.begin(), doorsLoc.end()}});
}