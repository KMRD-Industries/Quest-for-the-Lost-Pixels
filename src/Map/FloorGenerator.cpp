#include "FloorGenerator.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ranges>
#include <regex>

#include "Utils/Paths.h"

void FloorGenerator::generateFloor(const int h, const int w) { m_generator = DungeonGenerator(h, w); }

bool FloorGenerator::isConnected(const glm::ivec2& firstNode, const glm::ivec2& secondNode) const
{
    return m_generator.isConnected(firstNode, secondNode);
}

std::unordered_map<glm::ivec2, Room> FloorGenerator::getFloor(const bool generate)
{
    if (!generate) return m_floorMap;

    m_floorMap.clear();

    const auto avaibleMaps = getMapInfo();

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

std::vector<GameType::MapInfo> FloorGenerator::getMapInfo()
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    const std::string path = std::string(ASSET_PATH) + "/maps/";
    const std::regex pattern(R"(map_\d+\.json)");

    std::vector<GameType::MapInfo> mapInfo{};

    try
    {
        if (!fs::exists(path) || !fs::is_directory(path))
        {
            std::cerr << "Directory does not exist or is not a directory: " << path << std::endl;
            return mapInfo;
        }

        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                const std::string filename = entry.path().filename().string();
                const size_t underscorePos = filename.find_last_of("_");
                const size_t dotPos = filename.find_last_of(".");
                const std::string numberStr = filename.substr(underscorePos + 1, dotPos - underscorePos - 1);
                const int mapID = std::stoi(numberStr);

                if (std::regex_match(filename, pattern))
                {
                    std::cout << "Found matching file: " << filename << std::endl;

                    std::ifstream file(entry.path());
                    if (file.is_open())
                    {
                        json parsed_file;
                        file >> parsed_file;

                        for (auto& data : parsed_file["layers"])
                        {
                            for (auto& properties : data["properties"])
                            {
                                const std::string doorData = properties["value"];
                                std::vector<GameType::DoorEntraces> doorsLoc;
                                for (const auto& doorType : doorData)
                                {
                                    doorsLoc.push_back(static_cast<GameType::DoorEntraces>(doorType));
                                }
                                mapInfo.emplace_back(GameType::MapInfo{.mapID = mapID, .doorsLoc{doorsLoc}});
                            }
                        }
                    }
                    else
                    {
                        std::cerr << "Unable to open file: " << filename << std::endl;
                    }
                }
            }
        }
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
