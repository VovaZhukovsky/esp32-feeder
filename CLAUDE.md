# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

A PlatformIO-based Arduino firmware project targeting an ESP32 (DOIT ESP32 DEVKIT V1 board), using the Arduino framework. Currently a minimal blink sketch (`src/main.cpp`) that toggles the onboard LED on GPIO2 and logs over serial at 115200 baud.

## Commands

This project uses PlatformIO (`pio`), not the Arduino IDE/CLI.

- Build: `pio run`
- Upload to board: `pio run --target upload`
- Serial monitor: `pio device monitor` (115200 baud, matching `Serial.begin(115200)` in `main.cpp`)
- Build + upload + monitor in one step: `pio run --target upload --target monitor`
- Clean build artifacts: `pio run --target clean`
- Servo calibration sketch (`src/calibrate/`, separate env `servo-calibrate`): `pio run -e servo-calibrate --target upload --target monitor`
- Unit tests (PlatformIO Test Runner, none written yet — see `test/README`): `pio test`

The build environment is `esp32doit-devkit-v1`, defined in `platformio.ini`. There is currently only one environment, so `-e esp32doit-devkit-v1` is implied but can be passed explicitly if more environments are added later.

## Architecture

Standard PlatformIO project layout:

- `src/main.cpp` — firmware entry point (`setup()` / `loop()`). All application code currently lives here.
- `include/` — project header files (empty; see `include/README` for PlatformIO's usage convention).
- `lib/` — private, project-local libraries, each in its own subdirectory with PlatformIO's Library Dependency Finder auto-detecting `#include`s from `src/` (empty; see `lib/README`).
- `test/` — PlatformIO Test Runner unit tests (empty; see `test/README`).
- `platformio.ini` — build/board/framework configuration (single source of truth for target hardware).
- `.pio/` — build output directory, gitignored.

No external libraries are declared in `platformio.ini` yet (`lib_deps` is unset). When adding one, add it there rather than vendoring code manually.
