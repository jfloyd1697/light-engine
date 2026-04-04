#pragma once

#include <vector>
#include "light/Color.h"

namespace light {

class Canvas2D {
public:
    Canvas2D(int width, int height);

    void clear();
    void fill(Rgb c);
    void setPixel(int x, int y, Rgb c);
    Rgb getPixel(int x, int y) const;
    Rgb sampleNearest(float u, float v) const;

    int width() const noexcept { return m_width; }
    int height() const noexcept { return m_height; }
    const std::vector<Rgb>& pixels() const noexcept { return m_pixels; }

private:
    int m_width;
    int m_height;
    std::vector<Rgb> m_pixels;
};

} // namespace light
