#include "light/effects/PulseBlobEffect.h"

#include <cmath>

#include "light/Color.h"

namespace light::effects {

Rgb PulseBlobEffect::sample(float u, float v, uint32_t nowMs) const {
    const float du = u - centerU;
    const float dv = v - centerV;
    const float dist = std::sqrt(du * du + dv * dv);
    if (dist > radius) {
        return {0, 0, 0};
    }

    const float t = static_cast<float>(nowMs) / 1000.0f;
    const float phase = 0.5f * (1.0f + std::sin(2.0f * 3.14159265f * speedHz * t));
    const float radial = 1.0f - (dist / radius);
    const float gain = (minGain + (maxGain - minGain) * phase) * radial;
    return scale(color, gain);
}

} // namespace light::effects
