#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#pragma once

#include <chrono>
#include <deque>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include "glm/glm.hpp"

class DungeonGenerator
{
    using DirectedGraph = std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>>;
    using UnDirectedGraph = std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>>;
    using PathsNodes = std::unordered_map<std::string, std::unordered_set<glm::ivec2>>;
    using NodesPath = std::unordered_map<glm::ivec2, std::string>;
    using RoomCount = std::unordered_map<glm::ivec2, int>;
    using NodeEnterEdgesCount = std::unordered_map<glm::ivec2, int>;
    using NodeOutEdgesCount = std::unordered_map<glm::ivec2, int>;
    using PathNames = std::unordered_set<std::string>;

public:
    struct PathConfig
    {
        glm::ivec2 start{};
        std::string pathName{};
        int pathLength{};
    };
    struct sidePathConfig
    {
        std::string pathName{};
        std::string startingPathName{};
        std::string endPathName{};
        int minPathLength{};
        int maxPathLength{};
    };
    DungeonGenerator(int height, int width);
    void generateMainPath(int pathLength);
    void generateSidePath(const sidePathConfig& pathConfig);
    void makeLockAndKey();
    NodesPath getNodes();
    RoomCount getCount();
    std::optional<char> getLock(const glm::ivec2& node) const;
    std::optional<char> getKey(const glm::ivec2& node) const;
    NodeOutEdgesCount m_nodeOutEdgesCount;
    NodeEnterEdgesCount m_nodeEnterEdgesCount;

private:
    void generateMainPath(const PathConfig& pathConfig);
    std::optional<glm::ivec2> getRandomNeighbor(
        const glm::ivec2& current, const std::unordered_set<glm::ivec2>& nodesInPath,
        const std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>>& visitedNeighbors, int minL, int maxL,
        const std::string& endPath = "");

    void validateSidePathConfig(const sidePathConfig& pathConfig) const;
    void validateMainPath(const PathConfig& pathConfig) const;
    void findPlaceForKey(const glm::ivec2& lock);

    uint32_t m_height, m_width;
    PathsNodes m_paths;
    NodesPath m_nodeToPath;
    PathNames m_pathNames;
    DirectedGraph m_graph;
    UnDirectedGraph m_uGraph;
    RoomCount m_roomCount;
    int m_freeKey{};
    std::unordered_map<glm::ivec2, int> m_locks;
    std::unordered_map<glm::ivec2, int> m_keys;

    std::mt19937 gen;
};
