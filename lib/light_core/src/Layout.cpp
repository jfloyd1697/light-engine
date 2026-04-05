#include "light/Layout.h"

namespace light {

static const LedPoint kCpxRing[] = {
    LedPoint(0, 0.50f, 0.05f, 1.0f),
    LedPoint(1, 0.78f, 0.14f, 1.0f),
    LedPoint(2, 0.93f, 0.38f, 1.0f),
    LedPoint(3, 0.93f, 0.62f, 1.0f),
    LedPoint(4, 0.78f, 0.86f, 1.0f),
    LedPoint(5, 0.50f, 0.95f, 1.0f),
    LedPoint(6, 0.22f, 0.86f, 1.0f),
    LedPoint(7, 0.07f, 0.62f, 1.0f),
    LedPoint(8, 0.07f, 0.38f, 1.0f),
    LedPoint(9, 0.22f, 0.14f, 1.0f)
};

LayoutView circuitPlaygroundRingLayout() {
    return LayoutView(kCpxRing, sizeof(kCpxRing) / sizeof(kCpxRing[0]));
}

} // namespace light
