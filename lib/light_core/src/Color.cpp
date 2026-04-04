#include "light/Color.h"

#include <algorithm>

namespace light {

uint8_t clampByte(int value) {
    return static_cast<uint8_t>(std::clamp(value, 0, 255));
}

Rgb addSaturating(const Rgb& a, const Rgb& b) {
    return {
        clampByte(static_cast<int>(a.r) + static_cast<int>(b.r)),
        clampByte(static_cast<int>(a.g) + static_cast<int>(b.g)),
        clampByte(static_cast<int>(a.b) + static_cast<int>(b.b))
    };
}

Rgb scale(const Rgb& c, float factor) {
    return {
        clampByte(static_cast<int>(c.r * factor)),
        clampByte(static_cast<int>(c.g * factor)),
        clampByte(static_cast<int>(c.b * factor))
    };
}

} // namespace light
