#pragma once

#include "light/ILightField2D.h"

namespace light::effects {

class StripeScrollEffect : public ILightField2D {
public:
    Rgb primary{0, 60, 255};
    Rgb secondary{0, 0, 0};
    float stripeWidth{0.12f};
    float speed{0.20f};

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

} // namespace light::effects
