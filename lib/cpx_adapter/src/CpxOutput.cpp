#include "cpx/CpxOutput.h"

namespace cpx {

CpxOutput::CpxOutput() : m_strip(kPixelCount, kPixelPin, NEO_GRB + NEO_KHZ800) {}

void CpxOutput::begin() {
    m_strip.begin();
    m_strip.setBrightness(40);
    m_strip.show();
}

void CpxOutput::show(const light::Rgb* pixels, size_t count) {
    const size_t n = count > kPixelCount ? kPixelCount : count;
    for (size_t i = 0; i < n; ++i) {
        m_strip.setPixelColor(i, m_strip.Color(pixels[i].r, pixels[i].g, pixels[i].b));
    }
    for (size_t i = n; i < kPixelCount; ++i) {
        m_strip.setPixelColor(i, 0);
    }
    m_strip.show();
}

void CpxOutput::clear() {
    m_strip.clear();
    m_strip.show();
}

} // namespace cpx
