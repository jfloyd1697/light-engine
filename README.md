# Light Project with PlatformIO firmware and PC simulator

This repo contains:
- A PlatformIO firmware target for the Adafruit Circuit Playground Classic
- A desktop simulator you can run on the PC with keyboard-triggered animations

## Firmware

### Build and upload

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

### Serial commands

```text
PING
MODE ANIM
MODE MANUAL
PLAY PULSE
PLAY STRIPES
PLAY FLASH
PULSE_CENTER 0.50 0.50
SET_PIXEL 0 255 0 0
CLEAR
```

## PC simulator

The simulator is a standalone Python Tkinter app so it runs without extra GUI packages on most systems.

### Run

```bash
python tools/pc_light_sim/sim.py
```

### Keyboard controls

- `1` Pulse blob animation
- `2` Scrolling stripes animation
- `3` Muzzle flash animation
- `4` Manual mode demo pattern
- `Q` Move pulse up-left
- `W` Move pulse up
- `E` Move pulse up-right
- `A` Move pulse left
- `D` Move pulse right
- `Z` Move pulse down-left
- `X` Move pulse down
- `C` Move pulse down-right
- `[` decrease pulse radius
- `]` increase pulse radius
- `-` slower pulse
- `=` faster pulse
- `Space` trigger flash
- `M` toggle manual/animation mode
- `L` toggle LED overlay
- `Esc` quit

The simulator shows:
- the logical 2D light canvas
- the sampled LED positions for the Circuit Playground ring layout
- the current mode and active effect

## Layouts

- `layouts/cpx_ring.json` contains the normalized 2D layout for the 10 onboard NeoPixels.
