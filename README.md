# Game Boy tile renderer

A C++17 / Qt 6 modernization of a Spring 2024 computer architecture coursework repository. The modern viewer decodes Game Boy background tile graphics from video RAM snapshots and displays a deterministic synthetic demo. It is not a full playable emulator.

## Build and run
Install CMake, a C++17 compiler and Qt 6 Widgets (Ubuntu: qt6-base-dev; macOS: brew install qt cmake).

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/gameboy_viewer
```

On macOS, if Qt is not found, configure with -DCMAKE_PREFIX_PATH="$(brew --prefix qt)". Pass a snapshot path to display a static snapshot. To build only the portable renderer and tests, configure with -DBUILD_GUI=OFF.

## Design
The modern/ directory separates pure tile decoding from Qt presentation. The original coursework sources remain in place and are not built by the modern CMake target. See [architecture and interview notes](docs/INTERVIEW.md) and [original assignment](docs/ORIGINAL_ASSIGNMENT.md).

## Provenance
The original assignment supplied the Z80 implementation and Qt scaffolding. The modernization was AI-assisted in November 2026. No commercial ROM is needed for the synthetic demo. Build status is determined by GitHub Actions, not assumed.
