#pragma once
#include "ILightField2D.h"

namespace light {

class PulseBlobEffect : public ILightField2D {
public:
    float centerU;
    float centerV;
    float radius;
    float pulseHz;
    Rgb color;

    PulseBlobEffect()
        : centerU(0.5f),
          centerV(0.5f),
          radius(0.22f),
          pulseHz(1.2f),
          color(255, 80, 20) {}

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

class StripeFieldEffect : public ILightField2D {
public:
    float scrollHz;
    float stripeWidth;
    Rgb colorA;
    Rgb colorB;

    StripeFieldEffect()
        : scrollHz(0.4f),
          stripeWidth(0.14f),
          colorA(0, 0, 80),
          colorB(0, 80, 255) {}

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

} // namespace light
