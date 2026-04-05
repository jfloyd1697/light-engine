# Light Profile Project

This project includes:
- a JSON light profile for a muzzle-only LED ring
- a minimal JSON parser with no external dependency
- a `LightProfileLoader`
- a `LightAnimator` that evaluates the JSON animation kinds into canvas + LED output
- a `LightController` that triggers light events the same way audio events would be triggered
- a console demo that prints the resulting LED values over time

## Included animation kinds
- `fill`
- `off`
- `fade_out`
- `pulse`
- `flicker`
- `chase`
- `sequence`
- `repeat`

## Included action ops
- `play_animation`
- `stop_layer`
- `stop_all`

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/light_console_demo
```

On Windows with a multi-config generator, the executable may be under a configuration subfolder.

## Notes

The evaluator is intentionally compact. It is meant as a clean first pass for event-driven muzzle animations.
It does not yet include:
- alpha compositing beyond simple additive/replace behavior
- a separate region/mask database
- serialization back out to JSON
- a GUI editor

## Integration

Your runtime flow can look like this:

```cpp
lightController.trigger("fire", nowMs);
audioController.trigger("fire", nowMs);
```

That keeps light and audio event handling parallel.
