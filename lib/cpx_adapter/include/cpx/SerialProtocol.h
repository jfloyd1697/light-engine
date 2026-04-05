#pragma once
#include <Arduino.h>
#include "light/LightEngine.h"

namespace cpx {

class SerialProtocol {
public:
    void begin(light::LightEngine* engine);
    void update();

private:
    void processLine(const String& line);
    light::LightEngine* m_engine = nullptr;
    String m_buffer;
};

} // namespace cpx
