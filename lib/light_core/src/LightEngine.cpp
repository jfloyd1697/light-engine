#include "light/LightEngine.h"

#include "light/Rasterizer.h"

namespace light {

LightEngine::LightEngine(Layout layout)
    : m_layout(std::move(layout)) {
}

void LightEngine::setManualField(std::shared_ptr<ILightField2D> field) {
    m_manualField = std::move(field);
}

void LightEngine::setAnimationField(std::shared_ptr<ILightField2D> field) {
    m_animationField = std::move(field);
}

const ILightField2D* LightEngine::activeField() const {
    switch (m_mode) {
        case Mode::ManualField:
            return m_manualField.get();
        case Mode::Animation:
            return m_animationField.get();
    }
    return nullptr;
}

std::vector<Rgb> LightEngine::renderLeds(uint32_t nowMs) const {
    if (const auto* field = activeField()) {
        return sampleLayout(*field, m_layout, nowMs);
    }
    return std::vector<Rgb>(m_layout.size(), {0, 0, 0});
}

} // namespace light
