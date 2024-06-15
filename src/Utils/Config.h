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
    static constexpr float oneFrameTime{16.67};
    static constexpr int frameCycle{60};
    static constexpr int maximumNumberOfLayers{5};
    static constexpr float playerAttackRange{100.f};
    static constexpr float playerAttackDamage{100.f};
    static constexpr float playerAttackAngle{0.785f};
} // namespace config
