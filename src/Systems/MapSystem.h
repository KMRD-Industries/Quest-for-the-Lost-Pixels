#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"
#include "Utils/GameTypes.h"
#include "Utils/Helpers.h"

class MapSystem : public System
{
public:
    void loadMap(std::string&);
    std::vector<GameType::MapInfo> getMapInfo() const { return m_mapInfo; }

private:
    sf::Vector2f size{};
    std::vector<GameType::MapInfo> m_mapInfo{};

    static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);
    static std::string findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i);
};
