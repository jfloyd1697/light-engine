#include "cpx/CpxOutput.h"

namespace cpx {

CpxOutput::CpxOutput()
    : m_pixels(kPixelCount, kPixelPin, NEO_GRB + NEO_KHZ800) {
}

void CpxOutput::begin() {
    m_pixels.begin();
    m_pixels.setBrightness(40);
    m_pixels.show();
}

void CpxOutput::show(const light::Rgb* pixels, size_t count) {
    const size_t n = (count > kPixelCount) ? kPixelCount : count;
    for (size_t i = 0; i < n; ++i) {
        m_pixels.setPixelColor(i, m_pixels.Color(pixels[i].r, pixels[i].g, pixels[i].b));
    }
    for (size_t i = n; i < kPixelCount; ++i) {
        m_pixels.setPixelColor(i, 0);
    }
    m_pixels.show();
}

void CpxOutput::clear() {
    m_pixels.clear();
    m_pixels.show();
}

}
