#pragma once
#include <stdint.h>
#include "Color.h"

namespace light {

class ILightField2D {
public:
    virtual ~ILightField2D() {}
    virtual Rgb sample(float u, float v, uint32_t nowMs) const = 0;
};

} // namespace light
