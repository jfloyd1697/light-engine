#pragma once
#include <Arduino.h>
#include "light/LightEngine.h"
#include "light/Effects.h"

namespace cpx {

class SerialProtocol {
public:
    void begin(light::LightEngine* engine,
               light::PulseBlobEffect* pulse,
               light::StripeFieldEffect* stripes,
               light::FlashFieldEffect* flash);

    void update(uint32_t nowMs);

private:
    void processLine(const String& line, uint32_t nowMs);

    light::LightEngine* m_engine = nullptr;
    light::PulseBlobEffect* m_pulse = nullptr;
    light::StripeFieldEffect* m_stripes = nullptr;
    light::FlashFieldEffect* m_flash = nullptr;
    String m_buffer;
};

}
