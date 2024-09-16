#pragma once

#include "GameTypes.h"
#include "Helpers.h"
#include "Map.h"
#include "System.h"

class MapSystem : public System
{
public:
    void loadMap(const std::string&);

private:
    void doFlips(const std::uint8_t&, float&, sf::Vector2f&);
    void resetMap() const;
    void processTile(uint32_t, uint32_t, int, int, int, const Map&);

    sf::Vector2f getPosition(int, int, int);
    std::string findKeyLessThan(const std::unordered_map<std::string, long>&, long);

    enum FlipFlags : std::uint8_t
    {
        NoFlip = 0x0,
        VerticalFlip = 0x4, // 0100
        HorizontalFlip = 0x8, // 1000
        DiagonalFlip = 0x2, // 0010
        HorizontalVerticalFlip = 0xC, // 1100
        DiagonalVerticalFlip = 0x6, // 0110
        DiagonalHorizontalFlip = 0xA, // 1010
        AllFlips = 0xE // 1110
    };

    static constexpr float ROTATION_90 = 90.0f;
    static constexpr float ROTATION_180 = 180.0f;
    static constexpr float ROTATION_270 = 270.0f;
};
