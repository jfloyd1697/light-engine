#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace light {

struct LedPoint {
    uint16_t index{0};
    float u{0.0f};
    float v{0.0f};
    uint32_t regionMask{0};
    float brightnessScale{1.0f};
    std::string label;
};

class Layout {
public:
    Layout() = default;
    explicit Layout(std::vector<LedPoint> leds);

    const std::vector<LedPoint>& leds() const noexcept { return m_leds; }
    std::size_t size() const noexcept { return m_leds.size(); }

    static Layout makeCircuitPlaygroundRing();

private:
    std::vector<LedPoint> m_leds;
};

} // namespace light
