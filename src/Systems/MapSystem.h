#pragma once

#include "GameTypes.h"
#include "Helpers.h"
#include "Map.h"
#include "System.h"

class MapSystem : public System
{
public:
    void init();
    void update();
    void loadMap(const std::string&);

private:
    void doFlips(const std::uint8_t&, float&, sf::Vector2f&);
    void resetMap() const;
    void processTile(uint32_t, uint32_t, int, int, int, const Map&);

    sf::Vector2f getPosition(int, int, int) const;
    std::string findKeyLessThan(const std::unordered_map<std::string, long>&, long);
    std::uint32_t m_mask = 0xf0000000;
    uint32_t m_flipFlags{};
    uint32_t m_tileID{};
};
