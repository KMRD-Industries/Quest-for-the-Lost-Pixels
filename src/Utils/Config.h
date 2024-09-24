#pragma once
#include "glm/vec2.hpp"
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

#include <imgui.h>
#include <regex>
#include <string>
#include <unordered_map>

#include <unordered_map>

#include "GameTypes.h"
#include "TileComponent.h"
#include "Tileset.h"
#include "Types.h"

namespace config
{
    static constexpr bool debugMode{true};
    static constexpr float gameScale{3.f};
    static constexpr double meterToPixelRatio{30.f};
    static constexpr double pixelToMeterRatio{1 / 30.f};
    static constexpr float tileHeight{16.f};
    static constexpr float oneFrameTime{16.67};
    static constexpr int frameCycle{60};
    static constexpr int maximumNumberOfLayers{10};
    static constexpr float playerAttackRange{1000.f};
    static constexpr float playerAttackDamage{10.f};
    static constexpr float playerAttackAngle{0.785f};

    static constexpr int mapFirstEntity{1000};
    static constexpr int numberOfMapEntities{500};
    static constexpr int enemyFirstEntity{2000};
    static constexpr int numberOfEnemyEntities{100};
    static Entity playerEntity{1};
    static constexpr int playerAnimation{184};

    static constexpr int playerAcc{300};
    static constexpr int enemyAcc{25};

    static constexpr int startingRoomId{0};

    static constexpr int initWidth = {1920};
    static constexpr int initHeight = {1080};

    static const std::string backgroundColor{"#17205C"};

    static constexpr float maxCharacterHP{100};
    static constexpr float defaultCharacterHP{100};

    static constexpr float defaultEnemyKnockbackForce{300.f};
    static constexpr bool applyKnockback{false};

    // Text tag defaults
    static constexpr int textTagDefaultSize{20};
    static constexpr float textTagDefaultLifetime{60.0f};
    static constexpr float textTagDefaultSpeed{1.0f};
    static constexpr float textTagDefaultAcceleration{10.0f};
    static constexpr int textTagDefaultFadeValue{20};

    // Weapon component defaults
    static constexpr int weaponComponentDefaultDamageAmount{0};
    static constexpr bool weaponComponentDefaultIsAttacking{false};
    static constexpr bool weaponComponentDefaultQueuedAttack{false};
    static constexpr bool weaponComponentDefaultQueuedAttackFlag{false};

    static constexpr bool weaponComponentDefaultIsSwingingForward{true};
    static constexpr bool weaponComponentDefaultIsFacingRight{true};

    static constexpr float weaponComponentDefaultCurrentAngle{0.0f};
    static constexpr float weaponComponentDefaultInitialAngle{30.0f};
    static constexpr float weaponComponentDefaultRotationSpeed{15.0f};

    static constexpr float weaponComponentDefaultSwingDistance{90.0f};
    static constexpr float weaponComponentDefaultRemainingDistance{0.0f};
    static constexpr float weaponComponentDefaultRecoilAmount{10.0f};

    static constexpr glm::vec2 startingPosition{325.f, 325.f};
    static constexpr float spawnOffset{25};

    static const std::regex playerRegexTag{"^Player \\d+$"};
    static constexpr float invulnerabilityTimeAfterDMG{30.f};

    // Healthbar config
    static constexpr ImVec4 fullHPColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    static constexpr ImVec4 lowHPColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    static constexpr float ROTATION_90 = 90.0f;
    static constexpr float ROTATION_180 = 180.0f;
    static constexpr float ROTATION_270 = 270.0f;
    static constexpr int MAX_LEFT_FACING_ANGLE = 420;

    // Color balance structure
    struct ColorBalance
    {
        int redBalance{0};
        int greenBalance{0};
        int blueBalance{0};
    };

    // Function to convert floor ID to color string
    static std::string colorToString(int floorID)
    {
        switch (floorID)
        {
        case 0:
            return "#331541";
        case 1:
            return "#18215d";
        case 2:
            return "#25392e";
        default:
            return backgroundColor;
        }
    }

    // Maps
    static const std::unordered_map<int, std::string> m_mapFloorToTextureFile{{1, "CosmicLilac"}, {2, "Jungle"}};

    static const std::unordered_map<long, long> m_mapDungeonLevelToFloorInfo{{1, 1}, {2, 1}, {3, 1}, {4, 2}, {5, 2}};

    static const std::unordered_map<long, ColorBalance> m_mapColorScheme{
        {1, {25, 0, 0}}, {2, {0, 25, 0}}, {3, {0, 15, 15}}, {4, {45, 6, 35}}, {5, {15, 62, 35}}};


    enum class SpecialRoomTypes
    {
        NormalRoom,
        SpawnRoom,
        BossRoom
    };

    struct EnemyConfig
    {
        const std::string name{};
        const float hp{};
        const float damage{};
        const TileComponent textureData{};
        const Collision collisionData{};
    };

    const std::unordered_map<SpecialRoomTypes, char> prefixesForSpecialRooms{{SpecialRoomTypes::SpawnRoom, 's'},
                                                                             {SpecialRoomTypes::BossRoom, 'b'}};

    const std::unordered_multimap<Enemies::EnemyType, EnemyConfig> enemyData{
        {Enemies::EnemyType::MELEE,
         {.name = "Slime",
          .hp = 20.f,
          .damage = 5.f,
          .textureData{18, "AnimSlimes", 4},
          .collisionData{1, 8.5625, 13.24865, 16.375, 8.5227000004}}},
        {Enemies::EnemyType::BOSS,
         {.name = "Boss",
          .hp = 200.f,
          .damage = 30.f,
          .textureData{54, "AnimSlimes", 4},
          .collisionData{1, 8.5625, 13.24865, 16.375, 8.5227000004}}},
    };

    struct ItemConfig
    {
        const std::string name{};
        const float value{};
        const Items::Behaviours behaviour{};
        const TileComponent textureData{};
    };

    const std::vector<ItemConfig> itemsData{{"HPPotion", 10.f, Items::Behaviours::HEAL, {690, "Items", 4}},
                                            {"DMGPotion", 2.f, Items::Behaviours::DMGUP, {693, "Items", 4}}};
} // namespace config