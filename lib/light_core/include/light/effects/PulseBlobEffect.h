#pragma once

#include "light/ILightField2D.h"

namespace light::effects {

class PulseBlobEffect : public ILightField2D {
public:
    Rgb color{255, 100, 40};
    float centerU{0.5f};
    float centerV{0.5f};
    float radius{0.25f};
    float minGain{0.15f};
    float maxGain{1.0f};
    float speedHz{1.5f};

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

} // namespace light::effects
