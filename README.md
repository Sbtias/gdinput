# GDInput Buffer

A Geode mod prototype for Geometry Dash that queues press/release events and applies them after a configurable number of physics ticks.

## Controls

Press **F6** to cycle through:

| Mode | Regular mode | Wave |
|---|---:|---:|
| Stage 1 | 2 ticks | 2 ticks |
| Stage 2 | 4 ticks | 4 ticks |
| Stage 3 | 8 ticks | 8 ticks |
| Stage 4 | 16 ticks | 8 ticks |
| OFF | Raw input | Raw input |

The implementation treats one physics tick as approximately **1/240 second**. A delayed input buffer is not true prediction: it queues inputs and applies them later; it does not predict future player movement.

## Build

1. Install the Geode SDK and its build tools for your Geometry Dash version.
2. Set the `GEODE_SDK` environment variable to the SDK directory.
3. Configure and build with CMake, or use the Geode CLI if your SDK setup supports it.
4. Install the resulting `.geode` file into your Geometry Dash Geode mods folder.

## Compatibility and status

- Initial target: Windows, Geometry Dash 2.2081.
- This is an initial implementation and has **not yet been verified in-game**.
- Geode SDK versions can change hook signatures and metadata requirements. If the build fails, check the installed SDK's examples and headers before assuming the game is at fault.
- Delaying press and release events can change gameplay timing and may make controls feel less responsive at higher stages.
