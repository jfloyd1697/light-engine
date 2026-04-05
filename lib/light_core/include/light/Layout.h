#pragma once
#include <stddef.h>
#include <stdint.h>

namespace light {

struct LedPoint {
    uint16_t index = 0;
    float u = 0.0f;
    float v = 0.0f;
    float brightnessScale = 1.0f;
};

struct LayoutView {
    const LedPoint* points = nullptr;
    size_t count = 0;
};

LayoutView circuitPlaygroundRingLayout();

}
