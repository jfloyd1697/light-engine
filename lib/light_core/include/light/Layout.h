#pragma once
#include <stddef.h>
#include <stdint.h>

namespace light {

struct LedPoint {
    uint16_t index;
    float u;
    float v;
    float brightnessScale;

    LedPoint()
        : index(0), u(0.0f), v(0.0f), brightnessScale(1.0f) {}

    LedPoint(uint16_t idx, float uu, float vv, float scale)
        : index(idx), u(uu), v(vv), brightnessScale(scale) {}
};

struct LayoutView {
    const LedPoint* points;
    size_t count;

    LayoutView() : points(nullptr), count(0) {}
    LayoutView(const LedPoint* p, size_t c) : points(p), count(c) {}
};

LayoutView circuitPlaygroundRingLayout();

} // namespace light
