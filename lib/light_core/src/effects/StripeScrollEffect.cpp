#include "light/effects/StripeScrollEffect.h"

#include <cmath>

namespace light::effects {

Rgb StripeScrollEffect::sample(float u, float, uint32_t nowMs) const {
    const float t = static_cast<float>(nowMs) / 1000.0f;
    const float pos = u + speed * t;
    const float wrapped = pos - std::floor(pos);
    return (wrapped < stripeWidth) ? primary : secondary;
}

} // namespace light::effects
