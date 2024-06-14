#pragma once
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

namespace config
{
    static constexpr bool debugMode{false};
    static constexpr float gameScale{3.f};
    static constexpr double meterToPixelRatio{25.};
    static constexpr double pixelToMeterRatio{0.04};
    static constexpr float tileHeight{16.f};
    static constexpr float playerAttackRange{100.f};
    static constexpr float playerAttackDamage{100.f};
    static constexpr float playerAttackAngle{0.785f};

    static constexpr int mapFirstEntity{1000};
    static constexpr int numberOfMapEntities{500};
    static constexpr int enemyFirstEntity{2000};
    static constexpr int numberOfEnemyEntities{100};
    static constexpr int playerEntity{numberOfEnemyEntities + numberOfMapEntities};

    static constexpr int playerAcc = 150;
    static constexpr int enemyAcc = 50;

} // namespace config
