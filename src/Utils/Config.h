#pragma once
#include "glm/vec2.hpp"
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

namespace config
{
    static constexpr bool debugMode{true};
    static constexpr float gameScale{3.f};
    static constexpr double meterToPixelRatio{30.f};
    static constexpr double pixelToMeterRatio{1 / 30.f};
    static constexpr float tileHeight{16.f};
    static constexpr float oneFrameTime{16.67};
    static constexpr int frameCycle{60};
    static constexpr int maximumNumberOfLayers{5};
    static constexpr float playerAttackRange{100.f};
    static constexpr float playerAttackDamage{100.f};
    static constexpr float playerAttackAngle{0.785f};

    static constexpr int mapFirstEntity{1000};
    static constexpr int numberOfMapEntities{500};
    static constexpr int enemyFirstEntity{2000};
    static constexpr int numberOfEnemyEntities{100};
    static constexpr int playerEntity{numberOfEnemyEntities + numberOfMapEntities};

    static constexpr int playerAcc = 300;
    static constexpr int enemyAcc = 25;

    static const std::string backgroundColor{"#17205C"};

    static constexpr glm::vec2 startingPosition{325.f, 325.f};
    static constexpr float spawnOffset{25};
} // namespace config