#pragma once
// Every variable should be inline constexpr as it is file scope
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// Remember that in the game we use units in pixels, and the collision and physics system takes meters

namespace config
{
    static constexpr float gameScale{3.f};
    static constexpr float meterToPixelRatio{25.f};
} // namespace config
