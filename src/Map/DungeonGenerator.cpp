#include "DungeonGenerator.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <optional>

using randInt = std::uniform_int_distribution<int>;

DungeonGenerator::DungeonGenerator(const int height, const int width) :
    m_height(height), m_width(width), gen(std::chrono::system_clock::now().time_since_epoch().count())
{
}

void DungeonGenerator::generateMainPath(const int pathLength)
{
    const glm::ivec2 start{randInt(0, m_width - 1)(gen), randInt(0, m_height - 1)(gen)};
    generateMainPath({.start{start}, .pathName{"Main"}, .pathLength{pathLength}});
}

DungeonGenerator::NodesPath DungeonGenerator::getNodes() { return m_nodeToPath; }

DungeonGenerator::RoomCount DungeonGenerator::getCount() { return m_roomCount; }

DungeonGenerator::UnDirectedGraph DungeonGenerator::getGraph() const { return m_uGraph; }

std::optional<char> DungeonGenerator::getLock(const glm::ivec2& node) const
{
    if (m_locks.contains(node))
        return m_locks.at(node) + 'A';
    return std::nullopt;
}

std::optional<char> DungeonGenerator::getKey(const glm::ivec2& node) const
{
    if (m_keys.contains(node))
        return m_keys.at(node) + 'a';
    return std::nullopt;
}

glm::ivec2 DungeonGenerator::getStartingRoom() const { return m_startingRoom; }

glm::ivec2 DungeonGenerator::getBossRoom() const
{
    if (m_pathNames.contains("BossRoom"))
        return *m_paths.at("BossRoom").begin();
    std::cerr << "[WARNING] Path need to have name \"BossRoom\"\n";
    return m_startingRoom;
}

bool DungeonGenerator::isConnected(const glm::ivec2& firstNode, const glm::ivec2& secondNode) const
{
    if (m_uGraph.contains(firstNode))
        return m_uGraph.at(firstNode).contains(secondNode);
    return false;
}

void DungeonGenerator::generateMainPath(const PathConfig& pathConfig)
{
    std::deque<glm::ivec2> path;
    std::unordered_set<glm::ivec2> inPath;
    std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>> visitedNeighbors;

    path.push_back(pathConfig.start);
    inPath.insert(pathConfig.start);
    m_startingRoom = pathConfig.start;

    while (path.size() < pathConfig.pathLength)
    {
        auto current{path.back()};
        if (const auto neighbor{
            getRandomNeighbor(current, inPath, visitedNeighbors, pathConfig.pathLength, pathConfig.pathLength + 1)})
        {
            const auto nextNode{neighbor.value()};
            path.push_back(nextNode);
            inPath.insert(nextNode);
            visitedNeighbors[current].insert(nextNode);
        }
        else
        {
            if (current == pathConfig.start)
                throw std::runtime_error("Path generation failed");
            if (constexpr auto startingNode{1}; path.size() > startingNode)
            {
                visitedNeighbors[current].clear();
                path.pop_back();
                inPath.erase(current);
            }
        }
    }

    int nodeIndex = 1;
    for (const auto& pathNode : path)
    {
        m_pathNames.insert(pathConfig.pathName);
        m_nodeToPath[pathNode] = pathConfig.pathName;
        m_paths[pathConfig.pathName].insert(pathNode);
        m_roomCount[pathNode] = nodeIndex;
        ++nodeIndex;
    }
    for (int i = 0; i < path.size() - 1; i++)
    {
        m_graph[path[i]].insert(path[i + 1]);
        m_uGraph[path[i]].insert(path[i + 1]);
        m_uGraph[path[i + 1]].insert(path[i]);
        m_nodeEnterEdgesCount[path[i + 1]]++;
        m_nodeOutEdgesCount[path[i]]++;
    }
}

void DungeonGenerator::generateSidePath(const sidePathConfig& pathConfigToCheck)
{
    const auto pathConfig = validateAndRepairSidePathConfig(pathConfigToCheck);
    constexpr int startAndEndNodeOffset{2};
    const int minPathLength{startAndEndNodeOffset + pathConfig.minPathLength};
    const int maxPathLength{startAndEndNodeOffset + pathConfig.maxPathLength};

    std::vector<glm::ivec2> pathToSidePath{m_paths[pathConfig.startingPathName].begin(),
                                           m_paths[pathConfig.startingPathName].end()};

    std::ranges::shuffle(pathToSidePath, gen);

    auto start{pathToSidePath.front()};
    glm::ivec2 end;

    pathToSidePath.pop_back();
    auto startVal{m_roomCount[start]};
    auto endVal{std::numeric_limits<int>::max()};

    std::deque<glm::ivec2> path;
    std::unordered_set<glm::ivec2> inPath;
    std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>> visitedNeighbors;

    path.push_back(start);
    inPath.insert(start);

    while (true)
    {
        auto current{path.back()};

        if (auto neighbor{getRandomNeighbor(current, inPath, visitedNeighbors, minPathLength, maxPathLength,
                                            pathConfig.endPathName)};
            neighbor && path.size() <= maxPathLength)
        {
            const auto nextNode{neighbor.value()};
            path.push_back(nextNode);
            inPath.insert(nextNode);
            visitedNeighbors[current].insert(nextNode);
            if (m_nodeToPath[nextNode] == pathConfig.endPathName && path.size() >= minPathLength)
            {
                endVal = m_roomCount[nextNode];
                end = nextNode;
                break;
            }
        }
        else
        {
            if (constexpr auto startingNode{1}; path.size() > startingNode)
            {
                visitedNeighbors[current].clear();
                path.pop_back();
                inPath.erase(current);
            }
            else
            {
                if (pathToSidePath.empty())
                    throw std::runtime_error("Path generation failed, it is impossible :(");
                visitedNeighbors[current].clear();
                path.pop_back();
                inPath.erase(current);
                path.push_front(pathToSidePath.back());
                start = pathToSidePath.back();
                startVal = m_roomCount[pathToSidePath.back()];
                pathToSidePath.pop_back();
            }
        }
    }
    std::vector<glm::ivec2> sidePath;

    m_graph[start].insert(path[1]);

    m_uGraph[start].insert(path[1]);
    m_uGraph[path[1]].insert(start);


    if (startVal < endVal || endVal == 0)
    {
        if (!pathConfig.endPathName.empty())
        {
            m_nodeEnterEdgesCount[path.back()]++;
            path.pop_back();
            m_graph[path.back()].insert(end);
            m_uGraph[path.back()].insert(end);
            m_uGraph[end].insert(path.back());
        }
        m_nodeOutEdgesCount[path.front()]++;
        path.pop_front();
        sidePath = std::vector<glm::ivec2>(path.begin(), path.end());
    }
    else
    {
        if (!pathConfig.endPathName.empty())
        {
            m_nodeEnterEdgesCount[path.front()]++;
            path.pop_front();
            m_graph[path.back()].insert(end);
            m_uGraph[path.back()].insert(end);
            m_uGraph[end].insert(path.back());
        }
        m_nodeOutEdgesCount[path.back()]++;
        path.pop_back();
        sidePath = std::vector<glm::ivec2>(path.rbegin(), path.rend());
    }

    for (int i = 0; i < sidePath.size(); i++)
    {
        m_pathNames.insert(pathConfig.pathName);
        m_nodeToPath[sidePath[i]] = pathConfig.pathName;
        m_paths[pathConfig.pathName].insert(sidePath[i]);
        m_roomCount[sidePath[i]] = i + 1;
        m_nodeEnterEdgesCount[sidePath[i]]++;
    }
    for (int i = 0; i < sidePath.size() - 1; i++)
    {
        m_graph[sidePath[i]].insert(sidePath[i + 1]);
        m_nodeOutEdgesCount[sidePath[i]]++;
        m_uGraph[sidePath[i]].insert(sidePath[i + 1]);
        m_uGraph[sidePath[i + 1]].insert(sidePath[i]);
    }
    if (!pathConfig.endPathName.empty())
        m_nodeOutEdgesCount[sidePath.back()]++;
}

void DungeonGenerator::makeLockAndKey()
{
    std::deque<std::pair<glm::ivec2, int>> nodes(m_nodeOutEdgesCount.begin(), m_nodeOutEdgesCount.end());

    const auto sortByValue{[](const auto& a, const auto& b) { return a.second > b.second; }};

    std::ranges::shuffle(nodes, gen);
    std::ranges::sort(nodes, sortByValue);
    auto a = nodes;
    while (true)
    {
        const auto placeToLock{nodes.front()};
        if (m_locks.contains(placeToLock.first) ||
            (m_nodeToPath[placeToLock.first] == "Main" && m_roomCount[placeToLock.first] < 2) ||
            m_roomCount[placeToLock.first] < 2)
        {
            nodes.pop_front();
            continue;
        }
        m_locks[placeToLock.first] = m_freeKey++;
        findPlaceForKey(placeToLock.first);
        break;
    }
}

void DungeonGenerator::findPlaceForKey(const glm::ivec2& lock)
{
    std::vector<glm::ivec2> nodesToHideKey;
    std::deque<glm::ivec2> nodesToVisit;
    std::unordered_set<glm::ivec2> visited{lock};
    const auto currentPath{m_nodeToPath[lock]};
    for (const auto& neighbor : m_uGraph[lock])
    {
        if (m_nodeToPath[neighbor] != currentPath)
            continue;
        if (m_roomCount[neighbor] >= m_roomCount[lock])
            continue;
        nodesToHideKey.push_back(neighbor);
        nodesToVisit.push_back(neighbor);
        visited.insert(neighbor);
    }
    while (!nodesToVisit.empty())
    {
        const auto current{nodesToVisit.front()};
        nodesToVisit.pop_front();
        for (const auto& neighbor : m_uGraph[current])
        {
            if (visited.contains(neighbor))
                continue;
            nodesToHideKey.push_back(neighbor);
            nodesToVisit.push_back(neighbor);
            visited.insert(neighbor);
        }
    }
    std::ranges::shuffle(nodesToHideKey, gen);
    m_keys[nodesToHideKey.front()] = m_locks[lock];
    // TODO: priorytezacja kluczy w dead endach labiryntu
}

std::optional<glm::ivec2> DungeonGenerator::getRandomNeighbor(
    const glm::ivec2& current, const std::unordered_set<glm::ivec2>& nodesInPath,
    const std::unordered_map<glm::ivec2, std::unordered_set<glm::ivec2>>& visitedNeighbors, int minL, int maxL,
    const std::string& endPath)
{
    std::vector<glm::ivec2> neighbors;
    constexpr glm::ivec2 directions[]{{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    const auto checkIfNeighborIsCorrect{
        [this, &current, &nodesInPath, &visitedNeighbors, &neighbors, &endPath, &maxL, &minL](const auto& direction)
        {
            const auto neighbor{current + direction};
            const auto insideGraph{neighbor.x >= 0 && neighbor.x < m_width && neighbor.y >= 0 && neighbor.y < m_height};
            const auto isNotInOtherPath{m_nodeToPath[neighbor] == "" || m_nodeToPath[neighbor] == endPath};
            if (insideGraph && isNotInOtherPath)
            {
                const auto nodeInPath{nodesInPath.contains(neighbor)};
                const auto nodeVisited{visitedNeighbors.contains(current) &&
                    visitedNeighbors.at(current).contains(neighbor)};
                const auto correctPathLength{nodesInPath.size() < maxL};
                if (!nodeVisited && !nodeInPath && correctPathLength)
                {
                    if (!endPath.empty() && m_nodeToPath[neighbor] == endPath && nodesInPath.size() < minL)
                        return;
                    neighbors.push_back(neighbor);
                }
            }
        }};
    std::ranges::for_each(directions, checkIfNeighborIsCorrect);

    if (!neighbors.empty())
    {
        const auto nextNodeIndex{randInt(0, static_cast<int>(std::size(neighbors)) - 1)(gen)};
        return neighbors.at(nextNodeIndex);
    }

    return std::nullopt;
}

DungeonGenerator::sidePathConfig DungeonGenerator::validateAndRepairSidePathConfig(
    const sidePathConfig& pathConfig)
{
    if (m_pathNames.contains(pathConfig.pathName))
        throw std::logic_error("Path with this name already exists");
    if (pathConfig.maxPathLength < pathConfig.minPathLength)
        throw std::logic_error("Max path length must be greater than min path length");
    if (pathConfig.minPathLength < 0)
        throw std::logic_error("Path length can't be negative!");
    if (!m_pathNames.contains(pathConfig.startingPathName))
    {
        const int randomIndex = randInt(0, m_pathNames.size() - 1)(gen);

        auto it = m_pathNames.begin();
        std::advance(it, randomIndex);

        if (pathConfig.pathName != "BossRoom")
        {
            std::cout << std::format("[INFO] Path \"{}\" doesn't exist, choosing random path to start: \"{}\"\n",
                                     pathConfig.startingPathName, pathConfig.startingPathName);
        }

        sidePathConfig correctConfig = pathConfig;
        correctConfig.startingPathName = *it;

        return correctConfig;
    }
    return pathConfig;
}

void DungeonGenerator::validateMainPath(const PathConfig& pathConfig) const
{
    if (m_pathNames.contains(pathConfig.pathName))
        throw std::logic_error("Path with this name already exists");
    if (pathConfig.pathLength < 1)
        throw std::logic_error("Path length must be greater than 0");
    if (pathConfig.pathLength > m_width * m_height)
        throw std::logic_error("Path length is too long");
}