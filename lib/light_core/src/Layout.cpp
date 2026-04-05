#include "light/Layout.h"

namespace light {

static constexpr LedPoint kCpxRing[] = {
    {0, 0.50f, 0.05f, 1.0f},
    {1, 0.78f, 0.14f, 1.0f},
    {2, 0.93f, 0.38f, 1.0f},
    {3, 0.93f, 0.62f, 1.0f},
    {4, 0.78f, 0.86f, 1.0f},
    {5, 0.50f, 0.95f, 1.0f},
    {6, 0.22f, 0.86f, 1.0f},
    {7, 0.07f, 0.62f, 1.0f},
    {8, 0.07f, 0.38f, 1.0f},
    {9, 0.22f, 0.14f, 1.0f},
};

LayoutView circuitPlaygroundRingLayout() {
    return {kCpxRing, sizeof(kCpxRing) / sizeof(kCpxRing[0])};
}

}
