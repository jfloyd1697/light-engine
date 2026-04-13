#pragma once

#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

struct LedPoint {
    uint16_t index;
    float u;
    float v;
    float ledRadius;

    LedPoint() = default;
    LedPoint(uint16_t index_, float u_, float v_, float led_radius);
};

struct LayoutView {
    const LedPoint* points;
    size_t count;

    LayoutView() : points(nullptr), count(0) {}
    LayoutView(const LedPoint* p, size_t c) : points(p), count(c) {}
};

inline std::ostream& operator<<(std::ostream& os, const LedPoint& p) {
    os << "{ index: " << p.index
       << ", u: " << p.u
       << ", v: " << p.v
       << ", ledRadius: " << p.ledRadius
       << " }";
    return os;
}

inline std::string layoutViewToString(const LayoutView& view) {
    std::ostringstream ss;
    ss << "LayoutView {\n";
    ss << "  count: " << view.count << ",\n";
    ss << "  points: [\n";

    if (view.points != nullptr) {
        for (size_t i = 0; i < view.count; ++i) {
            ss << "    " << view.points[i];
            if (i + 1 < view.count) {
                ss << ",";
            }
            ss << "\n";
        }
    }

    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

inline void debugPrintLayoutView(const LayoutView& view, std::ostream& os = std::cout) {
    os << layoutViewToString(view) << "\n";
}