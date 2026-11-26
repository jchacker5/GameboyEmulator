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
#include "video.h"

namespace {

std::vector<unsigned char> rom;
unsigned char memoryRead(int address) {
    if (address >= 0 && static_cast<size_t>(address) < rom.size()) return rom[address];
    return 0;  // unmapped (the memory map is Part 3)
}

void memoryWrite(int, unsigned char) {}  // ROM is read-only; RAM arrives with the Part 3 memory map

bool loadRom(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;
    rom.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !file.bad() && !rom.empty();
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

    if (!cpu.halted) return 1;

    // Part 2: render the background layer of a video-memory snapshot.
    VideoSnapshot snapshot;
    std::ifstream video(vramPath);
    if (!readVideoSnapshot(video, snapshot)) {
        std::cerr << "Cannot read video snapshot " << vramPath << '\n';
        return 1;
    }
    QApplication app(argc, argv);
    Screen screen;
    screen.setFrame(renderBackground(snapshot));

    if (!pngPath.empty()) {
        if (!screen.saveFrame(pngPath.c_str())) {
            std::cerr << "Cannot write " << pngPath << '\n';
            return 1;
        }
        std::cout << "Wrote " << pngPath << '\n';
        return 0;
    }
    screen.show();
    return app.exec();
}
