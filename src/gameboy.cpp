// Game Boy emulator front end: loads a ROM into the Z80 core and renders the background layer of a
// video-memory snapshot. Parts 1-2 of the COMP 406 project (CPU execution, tile rendering).
//
// Usage: gameboy [--rom FILE] [--vram FILE] [--png OUT] [--quiet]
//   --png OUT  render one frame to OUT and exit (no window; use with QT_QPA_PLATFORM=offscreen)

#include <QApplication>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Z80.h"
#include "screen.h"

extern QApplication* app;

namespace {

std::vector<unsigned char> rom;
unsigned char graphicsRAM[8192];
int palette[4];
int tileset = 0, tilemap = 0, scrollx = 0, scrolly = 0;

constexpr int kScreenWidth = 160;
constexpr int kScreenHeight = 144;
constexpr int kTileMap0 = 0x1800;
constexpr int kTileMap1 = 0x1C00;

unsigned char memoryRead(int address) {
    if (address >= 0 && static_cast<size_t>(address) < rom.size()) return rom[address];
    return 0;  // unmapped (the memory map is Part 3)
}

void memoryWrite(int, unsigned char) {}  // ROM is read-only; RAM arrives with the Part 3 memory map

bool loadRom(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    rom.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !rom.empty();
}

// Snapshot format: 8192 VRAM bytes, then tileset, tilemap, scrollx, scrolly and 4 palette entries.
bool loadVideoSnapshot(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;
    for (unsigned char& byte : graphicsRAM) {
        int n;
        if (!(file >> n)) return false;
        byte = static_cast<unsigned char>(n);
    }
    return static_cast<bool>(file >> tileset >> tilemap >> scrollx >> scrolly >> palette[0] >> palette[1] >>
                             palette[2] >> palette[3]);
}

// Run the ROM until HALT, returning the instruction count.
long runUntilHalt(Z80& cpu, bool trace, long limit = 1'000'000) {
    long count = 0;
    while (!cpu.halted && count < limit) {
        cpu.doInstruction();
        ++count;
        if (trace) std::cout << "PC=" << cpu.PC << "  " << cpu.instruction << "  A=" << cpu.A << " B=" << cpu.B << '\n';
    }
    return count;
}

// Colour (0-3) of one screen pixel from the background layer: scroll, tile map lookup, tile data
// (signed indices for tileset 0), then the two bit-planes of the tile row, through the palette.
int backgroundPixel(int screenX, int screenY) {
    const int x = (screenX + scrollx) & 255;
    const int y = (screenY + scrolly) & 255;
    const int mapBase = tilemap == 0 ? kTileMap0 : kTileMap1;
    int tileIndex = graphicsRAM[mapBase + (y / 8) * 32 + (x / 8)];
    int tileAddress;
    if (tileset == 1) {
        tileAddress = tileIndex * 16;
    } else {
        if (tileIndex >= 128) tileIndex -= 256;
        tileAddress = 0x1000 + tileIndex * 16;
    }
    const unsigned char low = graphicsRAM[tileAddress + (y % 8) * 2];
    const unsigned char high = graphicsRAM[tileAddress + (y % 8) * 2 + 1];
    const int bit = 7 - (x % 8);
    const int colourIndex = (((high >> bit) & 1) << 1) | ((low >> bit) & 1);
    return palette[colourIndex] & 3;
}

void renderScreen() {
    for (int y = 0; y < kScreenHeight; ++y)
        for (int x = 0; x < kScreenWidth; ++x) updateSquare(x, y, backgroundPixel(x, y));
    onFrame();
}

}  // namespace

// Keyboard input is Part 5; screen.cpp calls these.
void keydown(int) {}
void keyup(int) {}

int main(int argc, char** argv) {
    std::string romPath = "assets/testrom.gb", vramPath = "assets/screendump.txt", pngPath;
    bool quiet = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto next = [&]() -> std::string {
            if (i + 1 >= argc) {
                std::cerr << arg << " needs a value\n";
                std::exit(2);
            }
            return argv[++i];
        };
        if (arg == "--rom") romPath = next();
        else if (arg == "--vram") vramPath = next();
        else if (arg == "--png") pngPath = next();
        else if (arg == "--quiet") quiet = true;
        else {
            std::cerr << "usage: gameboy [--rom FILE] [--vram FILE] [--png OUT] [--quiet]\n";
            return 2;
        }
    }

    setup(argc, argv, /*showWindow=*/pngPath.empty());

    // Part 1: run the test ROM on the Z80 core.
    if (!loadRom(romPath)) {
        std::cerr << "Cannot read ROM " << romPath << '\n';
        return 1;
    }
    Z80 cpu(memoryRead, memoryWrite);
    cpu.reset();
    const long executed = runUntilHalt(cpu, !quiet);
    std::cout << "Executed " << executed << " instructions; " << (cpu.halted ? "halted" : "did not halt")
              << ", A=" << cpu.A << '\n';

    // Part 2: render the background layer of a video-memory snapshot.
    if (!loadVideoSnapshot(vramPath)) {
        std::cerr << "Cannot read video snapshot " << vramPath << '\n';
        return 1;
    }
    renderScreen();

    if (!pngPath.empty()) {
        if (!saveFrame(pngPath.c_str())) {
            std::cerr << "Cannot write " << pngPath << '\n';
            return 1;
        }
        std::cout << "Wrote " << pngPath << '\n';
        return 0;
    }
    return app->exec();
}
