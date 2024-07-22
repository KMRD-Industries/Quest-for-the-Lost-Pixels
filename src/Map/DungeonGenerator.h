#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <chrono>
#include <optional>
#include <random>
#include <unordered_map>
#include <unordered_set>

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
    DungeonGenerator() = default;
    DungeonGenerator(int height, int width);
    void generateMainPath(int pathLength);
    void generateSidePath(const sidePathConfig& pathConfig);
    void makeLockAndKey();
    NodesPath getNodes();
    RoomCount getCount();
    [[nodiscard]] UnDirectedGraph getGraph() const;
    [[nodiscard]] std::optional<char> getLock(const glm::ivec2& node) const;
    [[nodiscard]] std::optional<char> getKey(const glm::ivec2& node) const;
    [[nodiscard]] glm::ivec2 getStartingRoom() const;
    [[nodiscard]] glm::ivec2 getEndingRoom() const;
    [[nodiscard]] bool isConnected(const glm::ivec2& firstNode, const glm::ivec2& secondNode) const;

private:
    void generateMainPath(const PathConfig& pathConfig);
    std::optional<glm::ivec2> getRandomNeighbor(
        const glm::ivec2& current, const std::unordered_set<glm::ivec2>& nodesInPath,
        const std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>>& visitedNeighbors, int minL, int maxL,
        const std::string& endPath = "");

    void validateSidePathConfig(const sidePathConfig& pathConfig) const;
    void validateMainPath(const PathConfig& pathConfig) const;
    void findPlaceForKey(const glm::ivec2& lock);

    int m_height{}, m_width{};
    PathsNodes m_paths{};
    NodesPath m_nodeToPath{};
    PathNames m_pathNames{};
    DirectedGraph m_graph{};
    UnDirectedGraph m_uGraph{};
    RoomCount m_roomCount{};
    int m_freeKey{};
    glm::ivec2 m_startingRoom{};
    glm::ivec2 m_endingRoom{};

    std::unordered_map<glm::ivec2, int> m_locks{};
    std::unordered_map<glm::ivec2, int> m_keys{};
    NodeOutEdgesCount m_nodeOutEdgesCount;
    NodeEnterEdgesCount m_nodeEnterEdgesCount;

    std::mt19937 gen{};
};
