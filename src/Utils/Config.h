#pragma once
#include "glm/vec2.hpp"
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

#include <SFML/Graphics.hpp>
#include <string>
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

    static constexpr int playerAcc = 300;
    static constexpr int enemyAcc = 25;

    static constexpr int initWidth = {1920};
    static constexpr int initHeight = {1080};

    static const std::string backgroundColor{"#17205C"};

    inline Entity playerEntity = {};
    static constexpr int playerAnimation{184};

    static constexpr float defaultCharacterHP{100};
    static constexpr float defaultEnemyHP{89};

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
} // namespace config
