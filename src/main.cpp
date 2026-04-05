#include <Arduino.h>
#include "light/LightEngine.h"
#include "cpx/CpxOutput.h"
#include "cpx/SerialProtocol.h"

namespace {
light::LightEngine g_engine;
cpx::CpxOutput g_output;
cpx::SerialProtocol g_protocol;
}

void setup() {
    Serial.begin(115200);
    size_t count = 0;
    const auto* layout = light::circuitPlaygroundRingLayout(count);
    g_engine.setLayout(layout, count);
    g_engine.setMode(light::EngineMode::Animation);
    g_engine.handleEvent(light::LightEvent::Idle);
    g_output.begin();
    g_protocol.begin(&g_engine);
    Serial.println("LIGHT READY");
}

void loop() {
    g_protocol.update();
    g_engine.update(millis());
    g_output.show(g_engine.pixels(), g_engine.pixelCount());
    delay(16);
}
