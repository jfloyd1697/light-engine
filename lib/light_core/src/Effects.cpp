#include "light/Effects.h"
#include <math.h>

namespace light {

static float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

Rgb PulseBlobEffect::sample(float u, float v, uint32_t nowMs) const {
    const float du = u - centerU;
    const float dv = v - centerV;
    const float dist = sqrtf(du * du + dv * dv);

    const float t = nowMs / 1000.0f;
    const float pulse = 0.5f + 0.5f * sinf(2.0f * 3.1415926f * pulseHz * t);

    float falloff = 1.0f - (dist / radius);
    falloff = clamp01(falloff);
    falloff *= falloff;

    const float intensity = clamp01(0.15f + pulse * falloff);
    return scale(color, intensity);
}

Rgb StripeFieldEffect::sample(float u, float, uint32_t nowMs) const {
    const float t = nowMs / 1000.0f;
    float x = u + t * scrollHz;
    x = x - floorf(x);

    const float period = stripeWidth * 2.0f;
    const float local = fmodf(x, period);
    return (local < stripeWidth) ? colorA : colorB;
}

} // namespace light
