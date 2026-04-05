#pragma once
#include "ILightField2D.h"

namespace light {

class PulseBlobEffect : public ILightField2D {
public:
    float centerU = 0.5f;
    float centerV = 0.5f;
    float radius = 0.22f;
    float pulseHz = 1.2f;
    Rgb color{255, 80, 20};

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

class StripeFieldEffect : public ILightField2D {
public:
    float scrollHz = 0.4f;
    float stripeWidth = 0.14f;
    Rgb colorA{0, 0, 80};
    Rgb colorB{0, 80, 255};

    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

class FlashFieldEffect : public ILightField2D {
public:
    float centerU = 0.85f;
    float centerV = 0.5f;
    float radius = 0.25f;
    uint32_t triggerMs = 0;
    uint16_t decayMs = 180;
    Rgb color{255, 220, 120};

    void trigger(uint32_t nowMs);
    Rgb sample(float u, float v, uint32_t nowMs) const override;
};

}
