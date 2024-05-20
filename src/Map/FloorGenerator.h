#pragma once
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "DungeonGenerator.h"
#include "GameTypes.h"
#include "Room.h"

class FloorGenerator
{
    const std::unordered_map<glm::ivec2, GameType::DoorEntraces> m_mapDirOnGraphToEntrace{
        {glm::ivec2{-1, 0}, GameType::DoorEntraces::WEST},
        {glm::ivec2{1, 0}, GameType::DoorEntraces::EAST},
        {glm::ivec2{0, 1}, GameType::DoorEntraces::NORTH},
        {glm::ivec2{0, -1}, GameType::DoorEntraces::SOUTH}};

public:
    FloorGenerator() = default;
    void generateFloor(int h, int w);
    void generateMainPath(const int mainPathLen) { m_generator.generateMainPath(mainPathLen); }
    void generateSidePath(const DungeonGenerator::sidePathConfig& path) { m_generator.generateSidePath(path); }
    void makeLockAndKey() { m_generator.makeLockAndKey(); }
    bool isConnected(const glm::ivec2& firstNode, const glm::ivec2& secondNode) const;

    std::unordered_map<glm::ivec2, Room> getFloor(bool generate);
    glm::ivec2 getStartingRoom() const { return m_generator.getStartingRoom(); }

private:
    std::vector<GameType::MapInfo> getMapInfo();
    static void checkSingleFile(const std::filesystem::directory_entry& entry, std::vector<GameType::MapInfo>& mapInfo);

    DungeonGenerator m_generator{};
    std::unordered_map<glm::ivec2, Room> m_floorMap{};

    /* This code will be used to show minimap of the dungeon
    void previevGenerator(sf::Font& font, std::vector<sf::RectangleShape>& rectangles, std::vector<sf::Text>& texts)
    {
    int h = 5;
    int w = 6;
    DungeonGenerator generator(h, w);
    generator.generateMainPath(11);
    generator.generateSidePath(
        {.pathName{"FirstC"}, .startingPathName{"Main"}, .endPathName{"Main"}, .minPathLength{3}, .maxPathLength{5}});
    generator.generateSidePath(
        {.pathName{"SecondC"}, .startingPathName{"Main"}, .endPathName{""}, .minPathLength{3}, .maxPathLength{5}});
    const int rectWidth = 50;
    const int rectHeight = 50;
    const int gap = 20;

    auto fullMap = generator.getNodes();
    auto roomCount = generator.getCount();
    generator.makeLockAndKey();

    for (int i = 0; i < w; ++i)
    {
        for (int j = 0; j < h; ++j)
        {
            sf::RectangleShape rectangle(sf::Vector2f(rectWidth, rectHeight));
            rectangle.setPosition(i * (rectWidth + gap), j * (rectHeight + gap));

            if (fullMap[{i, j}] == "Main")
            {
                rectangle.setFillColor(sf::Color::Red);
            }
            else if (fullMap[{i, j}] == "FirstC")
            {
                rectangle.setFillColor(sf::Color::Blue);
            }
            else if (fullMap[{i, j}] == "SecondC")
            {
                rectangle.setFillColor(sf::Color::Green);
            }
            else
            {
                rectangle.setFillColor(sf::Color::Black);
            }
            rectangles.push_back(rectangle);

            auto currentNodeVal = roomCount[{i, j}];
            sf::Text text;
            text.setFont(font);
            text.setString(std::to_string(currentNodeVal));
            text.setCharacterSize(14); // w pikselach
            text.setFillColor(sf::Color::White);
            text.setPosition(i * (rectWidth + gap), j * (rectHeight + gap));

            texts.push_back(text);


            sf::Text key;
            key.setFont(font);
            key.setString(std::to_string(generator.m_nodeOutEdgesCount[{i, j}]));
            key.setCharacterSize(14); // w pikselach
            key.setFillColor(sf::Color::Black);
            key.setPosition(i * (rectWidth + gap) + 30, j * (rectHeight + gap) + 30);
            texts.push_back(key);

            if (const auto text = generator.getKey({i, j}))
            {
                sf::Text key;
                key.setFont(font);
                key.setString(text.value());
                key.setCharacterSize(14); // w pikselach
                key.setFillColor(sf::Color::Black);
                key.setPosition(i * (rectWidth + gap) + 10, j * (rectHeight + gap) + 10);
                texts.push_back(key);
            }
            if (const auto text = generator.getLock({i, j}))
            {
                sf::Text key;
                key.setFont(font);
                key.setString(text.value());
                key.setCharacterSize(14); // w pikselach
                key.setFillColor(sf::Color::Black);
                key.setPosition(i * (rectWidth + gap) + 10, j * (rectHeight + gap) + 10);
                texts.push_back(key);
            }
        }
    }
    } */
};
