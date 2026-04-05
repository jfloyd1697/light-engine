#include "light/LightEngine.h"

namespace light {

LightEngine::LightEngine() = default;

void LightEngine::setLayout(LayoutView layout) {
    m_layout = layout;
    m_count = (layout.count > kMaxPixels) ? kMaxPixels : layout.count;
}

void LightEngine::setAnimationField(const ILightField2D* field) {
    m_animField = field;
}

void LightEngine::setMode(EngineMode mode) {
    m_mode = mode;
}

void LightEngine::setManualPixel(size_t index, Rgb color) {
    if (index >= kMaxPixels) return;
    m_manual[index] = color;
}

void LightEngine::clearManual() {
    for (size_t i = 0; i < kMaxPixels; ++i) {
        m_manual[i] = {};
    }
}

void LightEngine::render(uint32_t nowMs) {
    if (m_mode == EngineMode::Manual) {
        for (size_t i = 0; i < m_count; ++i) {
            m_pixels[i] = m_manual[i];
        }
        return;
    }

    for (size_t i = 0; i < m_count; ++i) {
        if (!m_animField) {
            m_pixels[i] = {};
            continue;
        }
        const auto& p = m_layout.points[i];
        m_pixels[i] = scale(m_animField->sample(p.u, p.v, nowMs), p.brightnessScale);
    }
}

}
