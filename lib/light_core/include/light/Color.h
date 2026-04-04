#pragma once

#include <cstdint>

namespace light {

struct Rgb {
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
};

Rgb addSaturating(const Rgb& a, const Rgb& b);
Rgb scale(const Rgb& c, float factor);
uint8_t clampByte(int value);

} // namespace light
