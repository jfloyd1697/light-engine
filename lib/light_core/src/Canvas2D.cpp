#include "light/Canvas2D.h"

#include <algorithm>
#include <cassert>

namespace light {

Canvas2D::Canvas2D(int width, int height)
    : m_width(width), m_height(height), m_pixels(static_cast<size_t>(width * height)) {
    assert(width > 0 && height > 0);
}

void Canvas2D::clear() {
    fill({0, 0, 0});
}

void Canvas2D::fill(Rgb c) {
    std::fill(m_pixels.begin(), m_pixels.end(), c);
}

void Canvas2D::setPixel(int x, int y, Rgb c) {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return;
    }
    m_pixels[static_cast<size_t>(y * m_width + x)] = c;
}

Rgb Canvas2D::getPixel(int x, int y) const {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return {};
    }
    return m_pixels[static_cast<size_t>(y * m_width + x)];
}

Rgb Canvas2D::sampleNearest(float u, float v) const {
    const int x = std::clamp(static_cast<int>(u * static_cast<float>(m_width - 1) + 0.5f), 0, m_width - 1);
    const int y = std::clamp(static_cast<int>(v * static_cast<float>(m_height - 1) + 0.5f), 0, m_height - 1);
    return getPixel(x, y);
}

} // namespace light
