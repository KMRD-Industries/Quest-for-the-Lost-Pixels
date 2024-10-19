#pragma once
#include <unordered_map>
#include <vector>
#include "Types.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_math.h"
#include "glm/gtx/hash.hpp"

#define M_PI 3.1415927f

namespace Sound
{
    enum class Type
    {
        MenuBackgroundMusic,
        GameBackgroundMusic
    };
}

namespace MenuStateMachine
{
    enum class StateAction
    {
        Push,
        Pop,
        PutOnTop
    };
}

namespace SpecialBlocks
{
    enum class Blocks : int
    {
        DOORSCOLLIDER = 0,
        STATICWALLCOLLIDER = 1,
        SPAWNERBLOCK = 2,
        STARTINGPOINT = 3,
        DOWNDOOR = 4,
        BOSSSPAWNERBLOCK = 5,
        CHESTSPAWNERBLOCK = 6
    };
} // namespace SpecialBlocks

namespace Items
{
    enum class Behaviours : int
    {
        HEAL = 0,
        DMGUP = 1
    };

    const std::unordered_map<std::string, Behaviours> behaviourMap = {
        {"Heal", Behaviours::HEAL},
        {"DmgUp", Behaviours::DMGUP},
    };
}

namespace Enemies
{
    enum class EnemyType : int
    {
        MELEE = 0,
        BOSS = 1
    };

    const std::unordered_map<std::string, EnemyType> enemyTypeMap = {
        {"Melee", EnemyType::MELEE},
        {"Boss", EnemyType::BOSS},
    };
} // namespace Enemies

inline sf::Vector2f operator+(const sf::Vector2f& sfVec, const glm::vec2& glmVec)
{
    return sf::Vector2f{sfVec.x + glmVec.x, sfVec.y + glmVec.y};
}

inline sf::Vector2f operator+(const sf::Vector2f& sfVec, const int x) { return sf::Vector2f{sfVec.x + x, sfVec.y + x}; }

namespace GameType
{
    struct MyVec2
    {
        float x, y;

        // Constructors
        MyVec2(const float x, const float y) : x{x}, y{y}
        {
        }

        MyVec2(const glm::vec2& vec) : x(vec.x), y(vec.y)
        {
        }

        operator glm::vec2() const { return {x, y}; }

        MyVec2(const sf::Vector2f& vec) : x(vec.x), y(vec.y)
        {
        }

        operator sf::Vector2f() const { return {x, y}; }

        MyVec2(const b2Vec2& vec) : x(vec.x), y(vec.y)
        {
        }

        operator b2Vec2() const { return {x, y}; }

        // Overloaded operators for vector addition
        MyVec2 operator+(const sf::Vector2f& vec) const { return MyVec2{x + vec.x, y + vec.y}; }

        MyVec2 operator+(const glm::vec2& vec) const { return MyVec2{x + vec.x, y + vec.y}; }

        MyVec2 operator+(const b2Vec2& vec) const { return MyVec2{x + vec.x, y + vec.y}; }

        MyVec2 operator+(const MyVec2& vec) const { return MyVec2{x + vec.x, y + vec.y}; }

        // Overloaded operators for vector multiplication
        MyVec2 operator*(const sf::Vector2f& vec) const { return MyVec2{x * vec.x, y * vec.y}; }

        MyVec2 operator*(const glm::vec2& vec) const { return MyVec2{x * vec.x, y * vec.y}; }

        MyVec2 operator*(const MyVec2& vec) const { return MyVec2{x * vec.x, y * vec.y}; }

        // Scalar multiplication
        MyVec2 operator*(const float scalar) const { return MyVec2{x * scalar, y * scalar}; }

        MyVec2 operator*(const double scalar) const
        {
            return MyVec2{x * static_cast<float>(scalar), y * static_cast<float>(scalar)};
        }

        // Compound assignment operators
        MyVec2& operator+=(const MyVec2& vec)
        {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        MyVec2& operator*=(const MyVec2& vec)
        {
            x *= vec.x;
            y *= vec.y;
            return *this;
        }

        MyVec2& operator+=(const sf::Vector2f& vec)
        {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        MyVec2& operator*=(const float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // Subtraction operator (handling negative direction)
        MyVec2 operator-() const { return MyVec2{-x, -y}; }
    };

    // Additional operators for sf::Vector2f and MyVec2
    inline sf::Vector2f operator+(const sf::Vector2f& sfVec, const MyVec2& myVec)
    {
        return sf::Vector2f{sfVec.x + myVec.x, sfVec.y + myVec.y};
    }

    inline sf::Vector2f operator*(const sf::Vector2f& sfVec, const MyVec2& myVec)
    {
        return sf::Vector2f{sfVec.x * myVec.x, sfVec.y * myVec.y};
    }

    // Add handling for glm::vec2 operations with MyVec2
    inline glm::vec2 operator+(const glm::vec2& glmVec, const MyVec2& myVec)
    {
        return glm::vec2{glmVec.x + myVec.x, glmVec.y + myVec.y};
    }

    inline glm::vec2 operator*(const glm::vec2& glmVec, const MyVec2& myVec)
    {
        return glm::vec2{glmVec.x * myVec.x, glmVec.y * myVec.y};
    }

    // Handle MyVec2 * int directly for cases like dir * -1
    inline MyVec2 operator*(const MyVec2& vec, const int scalar) { return MyVec2{vec.x * scalar, vec.y * scalar}; }

    inline MyVec2 operator*(const int scalar, const MyVec2& vec) { return MyVec2{scalar * vec.x, scalar * vec.y}; }

    inline MyVec2 operator+(const MyVec2& vec, const int scalar) { return MyVec2{vec.x + scalar, vec.y + scalar}; }

    enum class DoorEntraces : int
    {
        NORTH = static_cast<int>('N'),
        SOUTH = static_cast<int>('S'),
        WEST = static_cast<int>('W'),
        EAST = static_cast<int>('E')
    };

    enum class WeaponType : int
    {
        MELE = 1,
        WAND = 2,
        BOW = 3,
        UNKNOWN = 0
    };

    enum class ObjectType : int
    {
        NORMAL = 1,
        BULLET = 2
    };

    inline WeaponType stringToWeaponType(const std::string& type)
    {
        if (type == "mele")
            return WeaponType::MELE;
        if (type == "wand")
            return WeaponType::WAND;
        if (type == "bow")
            return WeaponType::BOW;
        return WeaponType::MELE; // Return unknown if no match found
    }

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

    const std::unordered_map<DoorEntraces, glm::ivec2> mapDoorsToGeo{{DoorEntraces::NORTH, {0, 1}},
                                                                     {DoorEntraces::SOUTH, {0, -1}},
                                                                     {DoorEntraces::EAST, {1, 0}},
                                                                     {DoorEntraces::WEST, {-1, 0}}};
    const std::unordered_map<glm::ivec2, DoorEntraces> geoToMapDoors{{{0, 1}, DoorEntraces::NORTH},
                                                                     {{0, -1}, DoorEntraces::SOUTH},
                                                                     {{1, 0}, DoorEntraces::EAST},
                                                                     {{-1, 0}, DoorEntraces::WEST}};

    struct MapInfo
    {
        std::string mapID{};
        std::vector<DoorEntraces> doorsLoc{};

        bool operator==(const MapInfo& other) const { return mapID == other.mapID; }
    };

    struct CollisionData
    {
        Entity entityID;
        std::string tag;
    };

    struct RaycastData
    {
        Entity entityID;
        std::string tag;
        MyVec2 position;
    };
} // namespace GameType

namespace std
{
    template <>
    struct hash<GameType::MapInfo>
    {
        size_t operator()(const GameType::MapInfo& mapInfo) const { return hash<std::string>()(mapInfo.mapID); }
    };
} // namespace std