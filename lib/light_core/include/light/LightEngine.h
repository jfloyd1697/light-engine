#pragma once
#include <stddef.h>
#include <stdint.h>
#include "Color.h"
#include "Layout.h"

namespace light {

class LightEngine {
public:
    static constexpr size_t kMaxPixels = 16;
    static constexpr int kCanvasWidth = 32;
    static constexpr int kCanvasHeight = 32;

    LightEngine();

    void setLayout(const LedPoint* points, size_t count);
    void clearPixels();
    void clearCanvas();
    void fillCanvas(Rgb c);
    void setCanvasPixel(int x, int y, Rgb c);
    void addCanvasPixel(int x, int y, Rgb c);
    Rgb sampleCanvas(float u, float v) const;
    void mapCanvasToPixels();

    const Rgb* pixels() const { return m_pixels; }
    size_t pixelCount() const { return m_count; }
    const Rgb* canvas() const { return m_canvas; }
    int canvasWidth() const { return kCanvasWidth; }
    int canvasHeight() const { return kCanvasHeight; }

private:
    const LedPoint* m_layout;
    size_t m_count;
    Rgb m_pixels[kMaxPixels];
    Rgb m_canvas[kCanvasWidth * kCanvasHeight];
};

}
