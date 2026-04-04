#pragma once

#include <memory>
#include <vector>

#include "light/ILightField2D.h"
#include "light/Layout.h"

namespace light {

class LightEngine {
public:
    enum class Mode {
        ManualField,
        Animation
    };

    explicit LightEngine(Layout layout);

    void setMode(Mode mode) noexcept { m_mode = mode; }
    void setManualField(std::shared_ptr<ILightField2D> field);
    void setAnimationField(std::shared_ptr<ILightField2D> field);

    std::vector<Rgb> renderLeds(uint32_t nowMs) const;
    Layout layout() const { return m_layout; }

private:
    const ILightField2D* activeField() const;

    Mode m_mode{Mode::Animation};
    Layout m_layout;
    std::shared_ptr<ILightField2D> m_manualField;
    std::shared_ptr<ILightField2D> m_animationField;
};

} // namespace light
