#pragma once
#include <array>
#include <cstdint>
#include <istream>
namespace gb {
constexpr int width=160, height=144;
struct Video {
 std::array<std::uint8_t,8192> ram{};
 int tileset=1, tilemap=0, scrollX=0, scrollY=0;
 std::array<int,4> palette{0,1,2,3};
};
using Frame=std::array<std::uint8_t,width*height>;
Frame render(const Video& video);
Video load(std::istream& input);
Video demo();
}
