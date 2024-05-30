#pragma once

#include "GameTypes.h"
#include "Helpers.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "System.h"

class MapSystem : public System
{
public:
    void loadMap(const std::string&);

private:
    sf::Vector2f size{};

    static void doFlips(std::uint8_t, float& rotation, sf::Vector2f& scale);
    static std::string findKeyLessThan(const std::unordered_map<std::string, long>& atlas_sets, long i);
    void resetMap();
    static void loadJsonFile(const std::string& basicString, nlohmann::json json);
};
