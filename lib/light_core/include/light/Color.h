#pragma once
#include <stdint.h>

namespace light {

struct Rgb {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

inline Rgb addSaturate(const Rgb& a, const Rgb& b) {
    const uint16_t rr = static_cast<uint16_t>(a.r) + b.r;
    const uint16_t gg = static_cast<uint16_t>(a.g) + b.g;
    const uint16_t bb = static_cast<uint16_t>(a.b) + b.b;
    return {
        static_cast<uint8_t>(rr > 255 ? 255 : rr),
        static_cast<uint8_t>(gg > 255 ? 255 : gg),
        static_cast<uint8_t>(bb > 255 ? 255 : bb)
    };
}

inline Rgb scale(const Rgb& c, float s) {
    if (s <= 0.0f) return {};
    if (s > 1.0f) s = 1.0f;
    return {
        static_cast<uint8_t>(c.r * s),
        static_cast<uint8_t>(c.g * s),
        static_cast<uint8_t>(c.b * s)
    };
}

}
