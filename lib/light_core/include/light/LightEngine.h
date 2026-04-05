#pragma once
#include <stddef.h>
#include <stdint.h>
#include "Color.h"
#include "ILightField2D.h"
#include "Layout.h"

namespace light {

enum class EngineMode {
    Animation,
    Manual
};

class LightEngine {
public:
    static constexpr size_t kMaxPixels = 16;

    LightEngine();

    void setLayout(LayoutView layout);
    void setAnimationField(const ILightField2D* field);
    void setMode(EngineMode mode);

    void setManualPixel(size_t index, Rgb color);
    void clearManual();

    void render(uint32_t nowMs);

    const Rgb* pixels() const { return m_pixels; }
    size_t pixelCount() const { return m_count; }

private:
    LayoutView m_layout{};
    const ILightField2D* m_animField = nullptr;
    EngineMode m_mode = EngineMode::Animation;
    Rgb m_pixels[kMaxPixels]{};
    Rgb m_manual[kMaxPixels]{};
    size_t m_count = 0;
};

}
