#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

#include "Config.h"
#include "GameTypes.h"
#include "glm/vec2.hpp"

struct PublicConfig
{
    //Engine settings
    bool debugMode{};
    float gameScale{};
    double meterToPixelRatio{};
    double pixelToMeterRatio{};
    float tileHeight{};
    float oneFrameTime{};
    int frameCycle{};
    int maximumNumberOfLayers{};
    int mapFirstEntity{};
    int numberOfMapEntities{};
    int enemyFirstEntity{};
    int numberOfEnemyEntities{};

    //Game Settings
    std::string backgroundColor{};
    bool applyKnockback{};

    //Dungeon Settings
    int maxDungeonDepth{};
    glm::vec2 startingPosition{};
    float spawnOffset{};

    //Player Stats
    float playerAttackRange{};
    float playerAttackDamage{};
    float playerAttackAngle{};
    uint32_t playerAnimation{};
    int playerAcc{};
    float maxCharacterHP{};
    float defaultCharacterHP{};
    float invulnerabilityTimeAfterDMG{};

    //Enemy Stats
    int enemyAcc{};
    float defaultEnemyKnockbackForce{};

    //Text tag settings
    int textTagDefaultSize{};
    float textTagDefaultLifetime{};
    float textTagDefaultSpeed{};
    float textTagDefaultAcceleration{};
    int textTagDefaultFadeValue{};

    //Weapon settings
    int weaponComponentDefaultDamageAmount{};
    bool weaponComponentDefaultIsAttacking{};
    bool weaponComponentDefaultQueuedAttack{};
    bool weaponComponentDefaultQueuedAttackFlag{};
    bool weaponComponentDefaultIsSwingingForward{};
    bool weaponComponentDefaultIsFacingRight{};
    float weaponComponentDefaultCurrentAngle{};
    float weaponComponentDefaultInitialAngle{};
    float weaponComponentDefaultRotationSpeed{};
    float weaponComponentDefaultSwingDistance{};
    float weaponComponentDefaultRemainingDistance{};
    float weaponComponentDefaultRecoilAmount{};
    int weaponInteractionDistance{};

    // Health bar settings
    ImVec4 fullHPColor{};
    ImVec4 lowHPColor{};

    // Enemies settings
    std::unordered_multimap<Enemies::EnemyType, config::EnemyConfig> enemyData{};

    // Items settings
    std::vector<config::ItemConfig> itemsData{};
};