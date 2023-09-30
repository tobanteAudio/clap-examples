#pragma once

#include <cstdint>

namespace ta {
template<typename T>
struct BasicColorRGBA
{
    T red;
    T green;
    T blue;
    T alpha;
};

using ColorRGBA      = BasicColorRGBA<std::uint8_t>;
using ColorFloatRGBA = BasicColorRGBA<float>;

[[nodiscard]] constexpr auto toFloat(ColorRGBA rgba) noexcept -> ColorFloatRGBA
{
    return {
        static_cast<float>(rgba.red) / 255.0F,
        static_cast<float>(rgba.green) / 255.0F,
        static_cast<float>(rgba.blue) / 255.0F,
        static_cast<float>(rgba.alpha) / 255.0F,
    };
}

struct Colors
{
    static constexpr auto black = ColorRGBA{0, 0, 0, 255};
    static constexpr auto white = ColorRGBA{255, 255, 255, 255};
    static constexpr auto gray  = ColorRGBA{25, 25, 25, 255};

    static constexpr auto red   = ColorRGBA{255, 0, 0, 255};
    static constexpr auto green = ColorRGBA{0, 255, 0, 255};
    static constexpr auto blue  = ColorRGBA{0, 0, 255, 255};
};
}  // namespace ta
