#pragma once

#include <array>
#include <cstdint>
#include <iosfwd>

constexpr int kScreenWidth = 160;
constexpr int kScreenHeight = 144;
using Frame = std::array<std::uint8_t, kScreenWidth * kScreenHeight>;

struct VideoSnapshot {
    std::array<std::uint8_t, 8192> ram{};
    std::array<int, 4> palette{0, 1, 2, 3};
    int tileset = 0;
    int tilemap = 0;
    int scrollx = 0;
    int scrolly = 0;
};

// 8192 byte values, tileset, tilemap, scroll and palette; optionally two
// complete four-shade sprite palettes from the course dump (currently unused).
// A failed parse leaves the caller's snapshot unchanged.
bool readVideoSnapshot(std::istream& input, VideoSnapshot& snapshot);
Frame renderBackground(const VideoSnapshot& snapshot);
