#include "light/Rasterizer.h"

namespace light {

Canvas2D rasterize(const ILightField2D& field, int width, int height, uint32_t nowMs) {
    Canvas2D canvas(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float u = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0f;
            const float v = height > 1 ? static_cast<float>(y) / static_cast<float>(height - 1) : 0.0f;
            canvas.setPixel(x, y, field.sample(u, v, nowMs));
        }
    }
    return canvas;
}

std::vector<Rgb> sampleLayout(const ILightField2D& field, const Layout& layout, uint32_t nowMs) {
    std::vector<Rgb> out;
    out.reserve(layout.size());
    for (const auto& led : layout.leds()) {
        out.push_back(scale(field.sample(led.u, led.v, nowMs), led.brightnessScale));
    }
    return out;
}

} // namespace light
