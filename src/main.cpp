#include <Arduino.h>
#include "light/Layout.h"
#include "light/Effects.h"
#include "light/LightEngine.h"
#include "cpx/CpxOutput.h"
#include "cpx/SerialProtocol.h"

light::PulseBlobEffect g_pulse;
light::StripeFieldEffect g_stripes;
light::FlashFieldEffect g_flash;
light::LightEngine g_engine;
cpx::CpxOutput g_output;
cpx::SerialProtocol g_protocol;

void setup() {
    Serial.begin(115200);

    g_engine.setLayout(light::circuitPlaygroundRingLayout());
    g_engine.setAnimationField(&g_pulse);
    g_engine.setMode(light::EngineMode::Animation);

    g_output.begin();
    g_protocol.begin(&g_engine, &g_pulse, &g_stripes, &g_flash);

    Serial.println("LIGHT READY");
}

void loop() {
    const uint32_t nowMs = millis();
    g_protocol.update(nowMs);
    g_engine.render(nowMs);
    g_output.show(g_engine.pixels(), g_engine.pixelCount());
    delay(16);
}
