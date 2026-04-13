#include "light/Color.h"


namespace light {

    Rgb addSaturating(const Rgb &a, const Rgb &b) {
        return {
            clampByte(static_cast<int>(a.r) + static_cast<int>(b.r)),
            clampByte(static_cast<int>(a.g) + static_cast<int>(b.g)),
            clampByte(static_cast<int>(a.b) + static_cast<int>(b.b))
        };
    }
} // namespace light
