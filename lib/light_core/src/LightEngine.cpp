#include "light/LightEngine.h"

namespace light {

static int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

LightEngine::LightEngine() : m_layout(nullptr), m_count(0) {
    clearPixels();
    clearCanvas();
}

void LightEngine::setLayout(const LedPoint* points, size_t count) {
    m_layout = points;
    m_count = (count < kMaxPixels) ? count : kMaxPixels;
}

void LightEngine::clearPixels() {
    for (size_t i = 0; i < kMaxPixels; ++i) {
        m_pixels[i] = Rgb(0, 0, 0);
    }
}

void LightEngine::clearCanvas() {
    fillCanvas(Rgb(0, 0, 0));
}

void LightEngine::fillCanvas(Rgb c) {
    for (int i = 0; i < kCanvasWidth * kCanvasHeight; ++i) {
        m_canvas[i] = c;
    }
}

void LightEngine::setCanvasPixel(int x, int y, Rgb c) {
    if (x < 0 || y < 0 || x >= kCanvasWidth || y >= kCanvasHeight) return;
    m_canvas[y * kCanvasWidth + x] = c;
}

void LightEngine::addCanvasPixel(int x, int y, Rgb c) {
    if (x < 0 || y < 0 || x >= kCanvasWidth || y >= kCanvasHeight) return;
    Rgb& dst = m_canvas[y * kCanvasWidth + x];
    dst = addSaturate(dst, c);
}

Rgb LightEngine::sampleCanvas(float u, float v) const {
    int x = clampInt(static_cast<int>(u * (kCanvasWidth - 1)), 0, kCanvasWidth - 1);
    int y = clampInt(static_cast<int>(v * (kCanvasHeight - 1)), 0, kCanvasHeight - 1);
    return m_canvas[y * kCanvasWidth + x];
}

void LightEngine::mapCanvasToPixels() {
    for (size_t i = 0; i < m_count; ++i) {
        if (!m_layout) {
            m_pixels[i] = Rgb(0, 0, 0);
            continue;
        }
        const LedPoint& p = m_layout[i];
        m_pixels[i] = scale(sampleCanvas(p.u, p.v), p.brightnessScale);
    }
}

}
