# Light Project

A hardware-agnostic light engine prototype built around a shared 2D light field.

## Goals

- One main lighting logic path for PC simulation and firmware targets.
- Effects render as a sampled 2D light field.
- Physical LED layouts are data-driven and map normalized coordinates to real LEDs.
- Manual control mode and preprogrammed animation mode share the same rendering pipeline.

## Structure

- `lib/light_core` Shared C++ light engine code.
- `tools/light_sim` Small console simulator that rasterizes the light field and prints LED samples.
- `firmware/cpx_light` Circuit Playground Classic firmware scaffold.
- `layouts` Example normalized 2D LED layouts.

## Build the simulator

```bash
cmake -S . -B build
cmake --build build
./build/tools/light_sim/light_sim
```

## Architecture

```text
Effect / Compositor
    -> ILightField2D sample(u, v, t)
        -> PC rasterizer (preview image)
        -> LED layout sampler (real pixels)
            -> output driver
```

## Next steps

- Add a JSON loader for layouts and effect presets.
- Add a real GUI simulator view.
- Add USB serial protocol for manual mode and animation mode.
- Port more effects and a compositor stack.
