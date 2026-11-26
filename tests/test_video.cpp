#include "video.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
unsigned checks;
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    std::cerr << __FILE__ << ':' << __LINE__ << ": " << #condition << '\n'; std::exit(1); \
} } while (0)

std::string encoded(const std::vector<std::string>& fields) {
    std::ostringstream result;
    for (const auto& field : fields) result << field << ' ';
    return result.str();
}

void parserTests() {
    std::vector<std::string> fields(8200, "0");
    fields[8192] = "1"; fields[8193] = "1";
    fields[8194] = "255"; fields[8195] = "255";
    fields[8196] = "3"; fields[8197] = "2"; fields[8198] = "1";
    fields[0] = "255";
    VideoSnapshot snapshot;
    std::istringstream valid(encoded(fields));
    CHECK(readVideoSnapshot(valid, snapshot));
    CHECK(snapshot.ram[0] == 255 && snapshot.tileset == 1 && snapshot.tilemap == 1);
    CHECK(snapshot.scrollx == 255 && snapshot.scrolly == 255 && snapshot.palette[0] == 3);
    for (auto index : {0, 8191, 8192, 8193, 8194, 8195, 8196, 8199}) {
        for (const auto* value : {"-1", "256", "junk", "999999999999999999"}) {
            auto invalid = fields;
            invalid[index] = value;
            std::istringstream input(encoded(invalid));
            CHECK(!readVideoSnapshot(input, snapshot));
            CHECK(snapshot.ram[0] == 255 && snapshot.scrollx == 255);
        }
    }
    for (int index : {8192, 8193, 8196, 8199}) {
        auto invalid = fields;
        invalid[index] = index < 8194 ? "2" : "4";
        std::istringstream input(encoded(invalid));
        CHECK(!readVideoSnapshot(input, snapshot));
    }
    for (const auto& text : {std::string(), encoded(fields) + "extra", encoded(fields) + "1"}) {
        std::istringstream input(text);
        CHECK(!readVideoSnapshot(input, snapshot));
    }
    std::istringstream legacy(encoded(fields) + "0 1 2 3 3 2 1 0");
    CHECK(readVideoSnapshot(legacy, snapshot));
    for (const auto* footer : {"0 1 2 3", "0 1 2 3 0 1 2 4", "0 1 2 3 0 1 2 3 0"}) {
        std::istringstream input(encoded(fields) + footer);
        CHECK(!readVideoSnapshot(input, snapshot));
    }
    fields.pop_back();
    std::istringstream truncated(encoded(fields));
    CHECK(!readVideoSnapshot(truncated, snapshot));
}

void signedTilesAndMaps() {
    const int indices[] = {128, 255, 0, 127};
    const int addresses[] = {0x0800, 0x0ff0, 0x1000, 0x17f0};
    for (int map = 0; map < 2; ++map) {
        for (int i = 0; i < 4; ++i) {
            VideoSnapshot snapshot;
            snapshot.tilemap = map;
            snapshot.palette = {3, 2, 1, 0};
            for (int row = 0; row < 8; ++row) {
                snapshot.ram[addresses[i] + 2 * row] = (i & 1) ? 255 : 0;
                snapshot.ram[addresses[i] + 2 * row + 1] = (i & 2) ? 255 : 0;
            }
            const int base = map == 0 ? 0x1800 : 0x1c00;
            std::fill_n(snapshot.ram.begin() + base, 1024, indices[i]);
            const auto frame = renderBackground(snapshot);
            for (auto pixel : frame) CHECK(pixel == 3 - i);
        }
    }
}

void scrollAndBitplanes() {
    VideoSnapshot snapshot;
    snapshot.tileset = 1;
    // Last tile on the map has one black pixel at its bottom-right corner.
    snapshot.ram[0x1bff] = 255;
    snapshot.ram[255 * 16 + 14] = 1;
    snapshot.ram[255 * 16 + 15] = 1;
    // First tile's top-left pixels are colour 1, 2, 3, 0.
    snapshot.ram[0] = 0xa0;
    snapshot.ram[1] = 0x60;
    snapshot.scrollx = 255; snapshot.scrolly = 255;
    const auto frame = renderBackground(snapshot);
    CHECK(frame[0] == 3);
    CHECK(frame[kScreenWidth + 1] == 1);
    CHECK(frame[kScreenWidth + 2] == 2);
    CHECK(frame[kScreenWidth + 3] == 3);
    CHECK(frame[kScreenWidth + 4] == 0);
    CHECK(frame[1] == 0);
    CHECK(frame[kScreenWidth] == 0);
    snapshot.tilemap = 1; // a separate, zero-filled map, with no tile 255
    CHECK(renderBackground(snapshot)[0] == 0);
}
}

int main() {
    parserTests(); signedTilesAndMaps(); scrollAndBitplanes();
    std::cout << "Video: " << checks << " checks passed\n";
}
