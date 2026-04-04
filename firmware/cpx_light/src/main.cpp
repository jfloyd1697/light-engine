// Circuit Playground Classic firmware scaffold
// Intended for Arduino IDE / PlatformIO style integration.

#include <stdint.h>

// Example future includes:
// #include <Adafruit_CircuitPlayground.h>
// #include "CircuitPlaygroundOutput.h"
// #include "SerialProtocol.h"
// #include "light/LightEngine.h"

namespace {

uint32_t nowMs() {
    // Replace with millis() in the real target.
    return 0;
}

} // namespace

void setup() {
    // Future work:
    // Serial.begin(115200);
    // CircuitPlayground.begin();
    // initialize light engine, layout, and default animation
}

void loop() {
    // Future work:
    // 1. Read serial commands from the PC
    // 2. Update mode / parameters
    // 3. Render LED colors using the shared light field model
    // 4. Push colors to the Circuit Playground NeoPixels
    (void)nowMs();
}
