# Shared Light Engine Project

This project contains:

- `lib/light_core`: shared C++ light engine used by both desktop and firmware
- `tools/desktop_sim`: desktop SDL2 simulator with keyboard-triggered animations
- `src/main.cpp`: PlatformIO firmware entry for Adafruit Circuit Playground Classic
- `lib/cpx_adapter`: firmware-specific NeoPixel and serial adapter

## Desktop simulator

Requirements:
- CMake 3.20+
- SDL2 development package
- C++17 compiler

Build:

```bash
cmake -S . -B build -DLIGHT_BUILD_DESKTOP=ON
cmake --build build
```

Run:

```bash
./build/desktop_light_sim
```

### Keyboard controls

- `1` Idle
- `2` Fire
- `3` Reload
- `4` Hit
- `5` Charge start
- `6` Charge stop
- `M` Manual mode
- `A` Animation mode
- `C` Clear manual LEDs
- Arrow keys move the cursor / hotspot
- `Space` trigger fire
- `Esc` quit

## Firmware (PlatformIO)

Board target is Circuit Playground Classic.

Build:

```bash
pio run
```

Upload:

```bash
pio run -t upload
```

Monitor:

```bash
pio device monitor -b 115200
```

### Serial commands

- `PING`
- `MODE ANIM`
- `MODE MANUAL`
- `TRIGGER IDLE`
- `TRIGGER FIRE`
- `TRIGGER RELOAD`
- `TRIGGER HIT`
- `TRIGGER CHARGE_START`
- `TRIGGER CHARGE_STOP`
- `CURSOR 0.5 0.5`
- `SET_PIXEL 0 255 0 0`
- `CLEAR`

## Notes

- The shared engine keeps a small RGB canvas for the desktop build and samples LED positions from that canvas.
- The Circuit Playground Classic only has 10 onboard NeoPixels and very limited RAM, so the current default canvas is intentionally modest.
- The desktop simulator and firmware use the same event and rendering logic from `light_core`.
