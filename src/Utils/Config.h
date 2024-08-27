#pragma once
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

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

} // namespace config
