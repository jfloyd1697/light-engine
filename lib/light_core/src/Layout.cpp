#include "light/Layout.h"

namespace light {

Layout::Layout(std::vector<LedPoint> leds)
    : m_leds(std::move(leds)) {
}

Layout Layout::makeCircuitPlaygroundRing() {
    return Layout({
        {0, 0.50f, 0.05f, 0, 1.0f, "top"},
        {1, 0.78f, 0.14f, 0, 1.0f, "upper_right"},
        {2, 0.93f, 0.38f, 0, 1.0f, "right_upper"},
        {3, 0.93f, 0.62f, 0, 1.0f, "right_lower"},
        {4, 0.78f, 0.86f, 0, 1.0f, "lower_right"},
        {5, 0.50f, 0.95f, 0, 1.0f, "bottom"},
        {6, 0.22f, 0.86f, 0, 1.0f, "lower_left"},
        {7, 0.07f, 0.62f, 0, 1.0f, "left_lower"},
        {8, 0.07f, 0.38f, 0, 1.0f, "left_upper"},
        {9, 0.22f, 0.14f, 0, 1.0f, "upper_left"}
    });
}

} // namespace light
