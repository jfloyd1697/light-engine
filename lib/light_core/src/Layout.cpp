#include "light/Layout.h"
#include <cmath>

namespace light {

static LedPoint kCpxRing[10];

static void initCpxRing() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    const float cx = 0.5f;
    const float cy = 0.5f;
    const float r = 0.38f;

    for (int i = 0; i < 10; ++i) {
        const float angle = (-3.1415926f / 2.0f) + (2.0f * 3.1415926f * i / 10.0f);
        const float u = cx + cosf(angle) * r;
        const float v = cy + sinf(angle) * r;
        const float ledRadius = 18.0f;
        kCpxRing[i] = LedPoint(static_cast<uint16_t>(i), u, v, ledRadius);
    }
}

LayoutView circuitPlaygroundRingLayout() {
    initCpxRing();
    return {kCpxRing, 10};
}

}
