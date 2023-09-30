#pragma once

namespace ta {
enum struct KeyModifier
{
    shift    = 0x01,
    control  = 0x02,
    alt      = 0x04,
    super    = 0x08,
    capsLock = 0x10,
    numLock  = 0x20,
};
}  // namespace ta
