#include "video.h"

#include <istream>

bool readVideoSnapshot(std::istream& input, VideoSnapshot& snapshot) {
    VideoSnapshot candidate;
    for (auto& byte : candidate.ram) {
        int value;
        if (!(input >> value) || value < 0 || value > 255) return false;
        byte = static_cast<std::uint8_t>(value);
    }
    if (!(input >> candidate.tileset >> candidate.tilemap >> candidate.scrollx >> candidate.scrolly
          >> candidate.palette[0] >> candidate.palette[1] >> candidate.palette[2] >> candidate.palette[3]))
        return false;
    if (candidate.tileset < 0 || candidate.tileset > 1 || candidate.tilemap < 0 || candidate.tilemap > 1 ||
        candidate.scrollx < 0 || candidate.scrollx > 255 || candidate.scrolly < 0 || candidate.scrolly > 255)
        return false;
    for (int shade : candidate.palette) if (shade < 0 || shade > 3) return false;
    input >> std::ws;
    // The course dump may append two four-entry sprite palettes (Part 6).
    // Accept that complete legacy footer, but reject partial/unknown trailing data.
    if (!input.eof()) {
        for (int i = 0; i < 8; ++i) {
            int shade;
            if (!(input >> shade) || shade < 0 || shade > 3) return false;
        }
        input >> std::ws;
    }
    if (!input.eof() || input.bad()) return false;
    snapshot = candidate;
    return true;
}

Frame renderBackground(const VideoSnapshot& snapshot) {
    Frame frame{};
    const int mapBase = snapshot.tilemap == 0 ? 0x1800 : 0x1c00;
    for (int screenY = 0; screenY < kScreenHeight; ++screenY) {
        for (int screenX = 0; screenX < kScreenWidth; ++screenX) {
            const int x = (screenX + snapshot.scrollx) & 255;
            const int y = (screenY + snapshot.scrolly) & 255;
            int tile = snapshot.ram[mapBase + (y / 8) * 32 + x / 8];
            int address;
            if (snapshot.tileset == 1) {
                address = tile * 16;
            } else {
                if (tile >= 128) tile -= 256;
                address = 0x1000 + tile * 16;
            }
            const auto low = snapshot.ram[address + (y % 8) * 2];
            const auto high = snapshot.ram[address + (y % 8) * 2 + 1];
            const int bit = 7 - x % 8;
            const int colour = (((high >> bit) & 1) << 1) | ((low >> bit) & 1);
            frame[screenY * kScreenWidth + screenX] = static_cast<std::uint8_t>(snapshot.palette[colour]);
        }
    }
    return frame;
}
