#pragma once

#include "light/Canvas2D.h"
#include "light/ILightField2D.h"
#include "light/Layout.h"

#include <vector>

namespace light {

Canvas2D rasterize(const ILightField2D& field, int width, int height, uint32_t nowMs);
std::vector<Rgb> sampleLayout(const ILightField2D& field, const Layout& layout, uint32_t nowMs);

} // namespace light
