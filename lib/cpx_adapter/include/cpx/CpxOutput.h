#pragma once
#include <Adafruit_NeoPixel.h>
#include "light/Color.h"

namespace cpx {

class CpxOutput {
public:
    static constexpr uint8_t kPixelPin = 17;
    static constexpr uint8_t kPixelCount = 10;

    CpxOutput();
    void begin();
    void show(const light::Rgb* pixels, size_t count);
    void clear();

private:
    Adafruit_NeoPixel m_strip;
};

} // namespace cpx
