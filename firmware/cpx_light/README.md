# Circuit Playground Classic Firmware Scaffold

This folder is a starting point for the Adafruit Circuit Playground Classic target.

## Intent

- Receive commands from the PC over USB serial.
- Run either manual field rendering or built-in animations.
- Sample the shared 2D light field model onto the 10 on-board NeoPixels.

## Suggested next step

Wrap the shared `light_core` interfaces with an Arduino-friendly adapter layer, then implement:

- `SerialProtocol` for command parsing
- `CircuitPlaygroundOutput` for NeoPixel writes
- a tiny layout constant matching `layouts/cpx_ring_layout.json`

## Notes

The Circuit Playground Classic is RAM-limited, so it is a better fit for direct LED sampling from field effects than for storing large dense RGB framebuffers.
