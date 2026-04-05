#pragma once
#include <stdint.h>

namespace light {

struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Rgb() : r(0), g(0), b(0) {}
    Rgb(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
};

inline uint8_t clampByte(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return static_cast<uint8_t>(v);
}

inline Rgb addSaturate(const Rgb& a, const Rgb& b) {
    return Rgb(
        clampByte(static_cast<int>(a.r) + static_cast<int>(b.r)),
        clampByte(static_cast<int>(a.g) + static_cast<int>(b.g)),
        clampByte(static_cast<int>(a.b) + static_cast<int>(b.b))
    );
}

inline Rgb scale(const Rgb& c, float s) {
    if (s <= 0.0f) return Rgb(0, 0, 0);
    if (s > 1.0f) s = 1.0f;
    return Rgb(
        static_cast<uint8_t>(c.r * s),
        static_cast<uint8_t>(c.g * s),
        static_cast<uint8_t>(c.b * s)
    );
}

}
