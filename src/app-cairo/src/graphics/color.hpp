#pragma once

#include <cstdint>

namespace mc {
struct Color
{
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t alpha;
};

[[nodiscard]] constexpr auto redAsFloat(Color color) noexcept -> float
{
    return static_cast<float>(color.red) / 255.0F;
}

[[nodiscard]] constexpr auto greenAsFloat(Color color) noexcept -> float
{
    return static_cast<float>(color.green) / 255.0F;
}

[[nodiscard]] constexpr auto blueAsFloat(Color color) noexcept -> float
{
    return static_cast<float>(color.blue) / 255.0F;
}

struct Colors
{
    static constexpr auto black = Color{0, 0, 0, 255};
    static constexpr auto white = Color{255, 255, 255, 255};
    static constexpr auto gray  = Color{25, 25, 25, 255};

    static constexpr auto red   = Color{255, 0, 0, 255};
    static constexpr auto green = Color{0, 255, 0, 255};
    static constexpr auto blue  = Color{0, 0, 255, 255};
};
}  // namespace mc
